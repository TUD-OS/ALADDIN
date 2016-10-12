/* Implementation of an Aladdin accelerator with a datapath that is connected
 * to main memory via DMA.
 */

#include <sstream>
#include <string>

#include "base/flags.hh"
#include "base/trace.hh"
#include "base/types.hh"
#include "base/misc.hh"
#include "mem/mem_object.hh"
#include "mem/packet.hh"
#include "mem/request.hh"
#include "sim/clocked_object.hh"
#include "sim/sim_exit.hh"
#include "sim/system.hh"

#include "aladdin/common/DatabaseDeps.h"

#include "aladdin/common/ScratchpadDatapath.h"
#include "aladdin/common/ExecNode.h"
#include "debug/DmaScratchpadDatapath.hh"
#include "DmaScratchpadDatapath.h"

DmaScratchpadDatapath::DmaScratchpadDatapath(
    const DmaScratchpadDatapathParams* params) :
    ScratchpadDatapath(params->benchName,
                       params->traceFileName,
                       params->configFileName,
                       params->spadPorts),
    Gem5Datapath(params,
                 params->acceleratorId,
                 params->executeStandalone,
                 params->system),
    inFlightNodes(0),
    _dataMasterId(params->system->getMasterId(name() + ".dmadata")),
    _ioCacheMasterId(params->system->getMasterId(name() + ".iocache")),
    spadPort(this, params->system, params->maxDmaRequests),
    cachePort(this),
    tickEvent(this),
    dmaSetupLatency(params->dmaSetupLatency)
{
  BaseDatapath::use_db = params->useDb;
  BaseDatapath::experiment_name = params->experimentName;
  BaseDatapath::cycleTime = params->cycleTime;
  std::stringstream name_builder;
  name_builder << "datapath" << accelerator_id;
  datapath_name = name_builder.str();
  setGlobalGraph();
  ScratchpadDatapath::globalOptimizationPass();
  setGraphForStepping();
  num_cycles = 0;
  system->registerAccelerator(accelerator_id, this, accelerator_deps);
  if (execute_standalone)
    scheduleOnEventQueue(1);
}

DmaScratchpadDatapath::~DmaScratchpadDatapath()
{
  delete scratchpad;
}

void
DmaScratchpadDatapath::event_step()
{
  step();
  scratchpad->step();
}

void
DmaScratchpadDatapath::stepExecutingQueue()
{
  auto it = executingQueue.begin();
  int index = 0;
  while (it != executingQueue.end())
  {
    ExecNode* node = *it;
    if (node->is_memory_op())
    {
      std::string node_part = node->get_array_label();
      if (registers.has(node_part) ||
          scratchpad->canServicePartition(node_part))
      {
        if (registers.has(node_part))
        {
          if (node->is_load_op())
            registers.getRegister(node_part)->increment_loads();
          else
            registers.getRegister(node_part)->increment_stores();
        }
        else
        {
          assert(scratchpad->addressRequest(node_part));
          if (node->is_load_op())
            scratchpad->increment_loads(node_part);
          else
            scratchpad->increment_stores(node_part);
        }
        markNodeCompleted(it, index);
      }
      else
      {
        ++it;
        ++index;
      }
    }
    else if (node->is_dma_op())
    {
      DmaRequestStatus status = Ready;
      MemAccess* mem_access = node->get_mem_access();
      Addr addr = mem_access->vaddr;
      unsigned size = mem_access->size;
      if (dma_requests.find(addr) == dma_requests.end())
        dma_requests[addr] = status;
      else
        status = dma_requests[addr];
      if (status == Ready && inFlightNodes < MAX_INFLIGHT_NODES)
      {
        //first time see, do access
        bool isLoad = node->is_dma_load();
        /* HACK!!! dmaStore sometimes goes out-of-order...schedule it until the
         * very end, fix this after ISCA. --Sophia*/
        if (isLoad || executedNodes >= totalConnectedNodes-5) {
          issueDmaRequest(addr, size, isLoad, node->get_node_id());
          dma_requests[addr] = Waiting;
          DPRINTF(DmaScratchpadDatapath, "node:%d is a dma request\n", node->get_node_id());
        }
        ++it;
        ++index;
      }
      else if (status == Returned)
      {
        markNodeCompleted(it, index);
        dma_requests.erase(addr);
        inFlightNodes--;
      }
      else
      {
        // Still waiting from the cache. Move on to the next node.
        ++it;
        ++index;
      }
    }
    else
    {
      // Not a memory operation node, so it can be completed in one cycle.
      markNodeCompleted(it, index);
    }
  }
}

bool
DmaScratchpadDatapath::step() {
  stepExecutingQueue();
  copyToExecutingQueue();
  DPRINTF(DmaScratchpadDatapath, "Cycles:%d, executedNodes:%d, totalConnectedNodes:%d\n",
     num_cycles, executedNodes, totalConnectedNodes);
  num_cycles++;
  if (executedNodes < totalConnectedNodes)
  {
    schedule(tickEvent, clockEdge(Cycles(1)));
    return true;
  }
  else
  {
    dumpStats();
    DPRINTF(DmaScratchpadDatapath, "Accelerator completed.\n");
    if (execute_standalone) {
      system->deregisterAccelerator(accelerator_id);
      if (system->numRunningAccelerators() == 0) {
        exitSimLoop("Aladdin called exit()");
      }
    } else {
      sendFinishedSignal();
    }
  }
  return false;
}

BaseMasterPort &
DmaScratchpadDatapath::getMasterPort(const string &if_name, PortID idx)
{
  // Get the right port based on name. This applies to all the
  // subclasses of the base CPU and relies on their implementation
  // of getDataPort and getInstPort. In all cases there methods
  // return a MasterPort pointer.
  if (if_name == "spad_port")
    return getDataPort();
  else if (if_name == "cache_port")
    return getIOCachePort();
  else
    return MemObject::getMasterPort(if_name);
}

/* Mark the DMA request node as having completed. */
void
DmaScratchpadDatapath::completeDmaAccess(Addr addr)
{
  DPRINTF(DmaScratchpadDatapath,
          "completeDmaAccess for addr:%#x \n", addr);
  dma_requests[addr] = Returned;
}

/* Issue a DMA request for memory. */
void
DmaScratchpadDatapath::issueDmaRequest(
    Addr addr, unsigned size, bool isLoad, int node_id)
{
  DPRINTF(DmaScratchpadDatapath, "issueDmaRequest for addr:%#x, size:%u\n", addr, size);
  MemCmd::Command cmd = isLoad ? MemCmd::ReadReq : MemCmd::WriteReq;
  Request::Flags flag = 0;
  uint8_t *data = new uint8_t[size];
  dmaQueue.push_back(addr);
  DmaEvent *dma_event = new DmaEvent(this);
  spadPort.dmaAction(cmd, addr, size, dma_event, data,
                     Cycles(dmaSetupLatency), flag);
}

void
DmaScratchpadDatapath::sendFinishedSignal()
{
  Flags<Packet::FlagsType> flags = 0;
  size_t size = 4;  // 32 bit integer.
  uint8_t *data = new uint8_t[size];
  // Set some sentinel value.
  for (int i = 0; i < size; i++)
    data[i] = 0x01;
  Request *req = new Request(finish_flag, size, flags, ioCacheMasterId());
  req->setThreadContext(context_id, thread_id);  // Only needed for prefetching.
  MemCmd::Command cmd = MemCmd::WriteReq;
  PacketPtr pkt = new Packet(req, cmd);
  pkt->dataStatic<uint8_t>(data);

  if (!cachePort.sendTimingReq(pkt))
  {
    assert(retryPkt == NULL);
    retryPkt = pkt;
    DPRINTF(DmaScratchpadDatapath,
            "Sending finished signal failed, retrying.\n");
  }
  else
  {
    DPRINTF(DmaScratchpadDatapath, "Sent finished signal.\n");
  }
}

void
DmaScratchpadDatapath::IOCachePort::recvRetry()
{
  assert(datapath->retryPkt != NULL);
  DPRINTF(DmaScratchpadDatapath, "recvRetry for address: %#x\n",
          datapath->retryPkt->getAddr());
  if (datapath->cachePort.sendTimingReq(datapath->retryPkt))
  {
    datapath->retryPkt = NULL;
    DPRINTF(DmaScratchpadDatapath, "Retry for control signal succeeded.\n");
  }
  else
  {
    DPRINTF(DmaScratchpadDatapath,
            "Retry for control signal failed, trying again.\n");
  }
}

bool
DmaScratchpadDatapath::IOCachePort::recvTimingResp(PacketPtr pkt)
{
  DPRINTF(DmaScratchpadDatapath, "Received timing response on IO cache.\n");
  delete pkt->senderState;
  delete pkt->req;
  delete pkt;
  return true;
}

bool DmaScratchpadDatapath::SpadPort::recvTimingResp(PacketPtr pkt)
{
  return DmaPort::recvTimingResp(pkt);
}

DmaScratchpadDatapath::DmaEvent::DmaEvent(DmaScratchpadDatapath *_dpath)
  : Event(Default_Pri, AutoDelete), datapath(_dpath) {}

void
DmaScratchpadDatapath::DmaEvent::process()
{
  assert(!datapath->dmaQueue.empty());
  datapath->completeDmaAccess(datapath->dmaQueue.front());
  datapath->dmaQueue.pop_front();
}

const char*
DmaScratchpadDatapath::DmaEvent::description() const
{
  return "DmaScratchpad DMA receving request event";
}

#ifdef USE_DB
int
DmaScratchpadDatapath::writeConfiguration(sql::Connection *con)
{
  int unrolling_factor, partition_factor;
  bool pipelining;
  getCommonConfigParameters(unrolling_factor, pipelining, partition_factor);

  sql::Statement *stmt = con->createStatement();
  stringstream query;
  query << "insert into configs (id, memory_type, trace_file, "
           "config_file, pipelining, unrolling, partitioning, "
           "max_dma_requests, dma_setup_latency) values (";
  query << "NULL" << ",\"spad\"" << "," << "\"" << trace_file << "\"" << ",\""
        << config_file << "\"," << pipelining << "," << unrolling_factor << ","
        << partition_factor << "," << spadPort.max_req << ","
        << dmaSetupLatency << ")";
  stmt->execute(query.str());
  delete stmt;
  // Get the newly added config_id.
  return getLastInsertId(con);
}
#endif

double
DmaScratchpadDatapath::getTotalMemArea()
{
  return scratchpad->getTotalArea();
}

void
DmaScratchpadDatapath::getAverageMemPower(
    unsigned int cycles, float *avg_power, float *avg_dynamic, float *avg_leak)
{
  scratchpad->getAveragePower(cycles, avg_power, avg_dynamic, avg_leak);
}

Addr
DmaScratchpadDatapath::getBaseAddress(std::string label)
{
  return (Addr) BaseDatapath::getBaseAddress(label);
}

void
DmaScratchpadDatapath::insertTLBEntry(
    Addr trace_addr, Addr vaddr, Addr paddr)
{
  fatal("DmaScratchpadDatapath does not use a TLB. Cannot insert TLB entry.\n");
}
////////////////////////////////////////////////////////////////////////////
//
//  The SimObjects we use to get the Datapath information into the simulator
//
////////////////////////////////////////////////////////////////////////////

DmaScratchpadDatapath*
DmaScratchpadDatapathParams::create()
{
  return new DmaScratchpadDatapath(this);
}
