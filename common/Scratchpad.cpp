#include "Scratchpad.h"

AladdinScratchpad::AladdinScratchpad(
    unsigned ports_per_part, float cycle_time, bool _ready_mode) {
  num_ports = ports_per_part;
  cycleTime = cycle_time;
  ready_mode = _ready_mode;
}

AladdinScratchpad::~AladdinScratchpad() { clear(); }

void AladdinScratchpad::clear() {
  for (auto it = logical_arrays.begin(); it != logical_arrays.end(); ++it) {
    delete it->second;
  }
  logical_arrays.clear();
}

// wordsize in bytes
void AladdinScratchpad::setScratchpad(std::string baseName,
                               Addr base_addr,
                               PartitionType part_type,
                               unsigned part_factor,
                               unsigned num_of_bytes,
                               unsigned wordsize) {
  assert(!partitionExist(baseName));
  LogicalArray* curr_base = new LogicalArray(
      baseName, base_addr, part_type, part_factor, num_of_bytes, wordsize,
      num_ports, ready_mode);
  logical_arrays[baseName] = curr_base;
}

void AladdinScratchpad::step() {
  for (auto it = logical_arrays.begin(); it != logical_arrays.end(); ++it)
    it->second->step();
}

bool AladdinScratchpad::partitionExist(std::string baseName) {
  auto partition_it = logical_arrays.find(baseName);
  if (partition_it != logical_arrays.end())
    return true;
  else
    return false;
}

bool AladdinScratchpad::canService() {
  for (auto it = logical_arrays.begin(); it != logical_arrays.end(); ++it) {
    if (it->second->canService())
      return true;
  }
  return false;
}

bool AladdinScratchpad::canServicePartition(std::string baseName,
                                     unsigned part_index,
                                     Addr addr,
                                     bool isLoad) {
  return getLogicalArray(baseName)->canService(part_index, addr, isLoad);
}

// power in mW, energy in pJ, time in ns
void AladdinScratchpad::getAveragePower(unsigned int cycles,
                                 float* avg_power,
                                 float* avg_dynamic,
                                 float* avg_leak) {
  float load_energy = 0;
  float store_energy = 0;
  float leakage_power = 0;
  for (auto it = logical_arrays.begin(); it != logical_arrays.end(); ++it) {
    load_energy += it->second->getReadEnergy();
    store_energy += it->second->getWriteEnergy();
    leakage_power += it->second->getLeakagePower();
  }

  // Load power and store power are computed per cycle, so we have to average
  // the aggregated per cycle power.
  *avg_dynamic = (load_energy + store_energy) / (cycleTime * cycles);
  *avg_leak = leakage_power;
  *avg_power = *avg_dynamic + *avg_leak;
}

float AladdinScratchpad::getTotalArea() {
  float mem_area = 0;
  for (auto it = logical_arrays.begin(); it != logical_arrays.end(); ++it)
    mem_area += it->second->getArea();
  return mem_area;
}

void AladdinScratchpad::getMemoryBlocks(std::vector<std::string>& names) {
  for (auto it = logical_arrays.begin(); it != logical_arrays.end(); ++it) {
    names.push_back(it->first);
  }
}
void AladdinScratchpad::increment_loads(std::string array_label, unsigned index) {
  getLogicalArray(array_label)->increment_loads(index);
}

void AladdinScratchpad::increment_stores(std::string array_label, unsigned index) {
  getLogicalArray(array_label)->increment_stores(index);
}

void AladdinScratchpad::increment_dma_loads(std::string array_label,
                                     unsigned dma_size) {
  getLogicalArray(array_label)->increment_streaming_stores(dma_size);
}

void AladdinScratchpad::increment_dma_stores(std::string array_label,
                                      unsigned dma_size) {
  getLogicalArray(array_label)->increment_streaming_loads(dma_size);
}
