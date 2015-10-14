from m5.params import *
from MemObject import MemObject
from m5.proxy import *

class HybridDatapath(MemObject):
  type = "HybridDatapath"
  cxx_header = "aladdin/gem5/HybridDatapath.h"
  benchName = Param.String("Aladdin Bench Name")
  traceFileName = Param.String("Aladdin Input Trace File")
  configFileName = Param.String("Aladdin Config File")
  cycleTime = Param.Unsigned(1, "Clock Period: 1ns default")
  acceleratorName = Param.String("", "Unique accelerator name")
  acceleratorId = Param.Int(-1, "Accelerator Id")
  system = Param.System(Parent.any, "system object")
  executeStandalone = Param.Bool(True, "Execute Aladdin standalone, without a "
      "CPU/user program.")
  useDb = Param.Bool(False, "Store results in database.")
  experimentName = Param.String("NULL", "Experiment name. String identifier "
      "for a set of related simulations.")

  # Scratchpad/DMA parameters.
  dmaSetupLatency = Param.Unsigned(1000, "DMA Setup Latency")
  maxDmaRequests = Param.Unsigned(16, "Max number of outstanding DMA requests")
  spadPorts = Param.Unsigned(1, "Scratchpad ports per partition")

  # Cache parameters.
  cacheSize = Param.String("16kB", "Private cache size")
  cacheLineSize = Param.Int("64", "Cache line size (in bytes)")
  cacheAssoc = Param.Int(1, "Private cache associativity")
  l2cacheSize = Param.String("128kB", "Shared L2 cache size")
  cacheHitLatency = Param.Int(1, "Hit latency")
  cactiCacheConfig = Param.String("", "CACTI cache config file")
  tlbEntries = Param.Int(0, "number entries in TLB (0 implies infinite)")
  tlbAssoc = Param.Int(4, "Number of sets in the TLB")
  tlbHitLatency = Param.Cycles(0, "number of cycles for a hit")
  tlbMissLatency = Param.Cycles(10, "number of cycles for a miss")
  tlbPageBytes = Param.Int(4096, "Page Size")
  tlbCactiConfig = Param.String("", "TLB CACTI configuration file.")
  isPerfectTLB = Param.Bool(False, "Is this TLB perfect (e.g. always hit)")
  numOutStandingWalks = Param.Int(4, "num of outstanding page walks")
  loadQueueSize = Param.Int(16, "Size of the load queue.")
  loadBandwidth = Param.Int(
      1, "Number of loads that can be issued per cycle.")
  loadQueueCacheConfig = Param.String("", "Load queue CACTI config file.")
  storeQueueSize = Param.Int(16, "Size of the store queue.")
  storeBandwidth = Param.Int(
      1, "Number of stores that can be issued per cycle.")
  storeQueueCacheConfig = Param.String("", "Store queue CACTI config file.")
  tlbBandwidth = Param.Int(
      1, "Number of translations that can be requested per cycle.")

  enableStatsDump = Param.Bool(
      False, "Dump m5 stats after each accelerator invocation.")

  spad_port = MasterPort("HybridDatapath DMA port")
  cache_port = MasterPort("HybridDatapath cache coherent port")
  # HACK: We don't have a scratchpad object. Currently we just connect the
  # scratchpad port inside datapath directly to the memory bus.
  def connectPrivateScratchpad(self, bus):
    self.spad_port = bus.slave

  def addPrivateL1Dcache(self, cache, bus, dwc = None) :
    self.cache = cache
    self.cache_port = cache.cpu_side
    self.cache.mem_side  = bus.slave
