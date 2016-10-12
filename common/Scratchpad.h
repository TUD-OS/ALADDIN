#ifndef __SCRATCHPAD__
#define __SCRATCHPAD__

#include "Partition.h"
#include "LogicalArray.h"

/* Definitions of three classes for Scratchpad processing.
 *
 *   Scratchpad     : Top level class for the entire scratchpad system. It can
 *                  : contain multiple LogicalArrays if the program accesses
 *                  : multiple arrays.
 *
 *   LogicalArray   : Defines the base information of an array, which can be
 *                  : further partitioned into multiple Partitions.
 *
 *   Partition      : Represents each partition of a scratchpad.
*/

class Scratchpad {
 public:
  Scratchpad(unsigned p_ports_per_part, float cycle_time, bool ready_mode);
  virtual ~Scratchpad();
  void clear();
  void step();
  void setScratchpad(std::string baseName,
                     Addr base_addr,
                     PartitionType part_type,
                     unsigned part_factor,
                     unsigned num_of_bytes,
                     unsigned wordsize);
  bool canService();
  bool canServicePartition(std::string baseName,
                           unsigned index,
                           Addr addr,
                           bool isLoad);
  bool partitionExist(std::string baseName);

  size_t getPartitionIndex(std::string arrayName, Addr abs_addr) {
    return logical_arrays[arrayName]->getPartitionIndex(abs_addr);
  }

  void readData(std::string arrayName, Addr addr, size_t len, uint8_t* data) {
    logical_arrays[arrayName]->readData(addr, len, data);
  }

  void writeData(std::string arrayName, Addr addr, uint8_t* data, size_t len) {
    logical_arrays[arrayName]->writeData(addr, data, len);
  }

  /* Set the ready bit for a specific addr. */
  void setReadyBit(
           std::string baseName, unsigned part_index, Addr addr) {
    logical_arrays[baseName]->setReadyBit(part_index, addr);
  }
  /* Reset the ready bit for a specific addr. */
  void resetReadyBit(
           std::string baseName, unsigned part_index, Addr addr) {
    logical_arrays[baseName]->resetReadyBit(part_index, addr);
  }

  /* Set/reset multiple ready bits given then address and the data length. */
  void setReadyBitRange(
           std::string baseName, Addr addr, unsigned size) {
    logical_arrays[baseName]->setReadyBitRange(addr, size);
  }

  void resetReadyBitRange(
           std::string baseName, Addr addr, unsigned size) {
    logical_arrays[baseName]->resetReadyBitRange(addr, size);
  }

  /* Set all the ready bits for the baseName array. */
  void setReadyBits(std::string baseName) {
    logical_arrays[baseName]->setReadyBits();
  }
  /* Reset all the ready bits for the baseName array. */
  void resetReadyBits(std::string baseName) {
    logical_arrays[baseName]->resetReadyBits();
  }

  /* Set all the ready bits in all the scratchpads. */
  void setReadyBits() {
    for (auto it = logical_arrays.begin(); it != logical_arrays.end(); ++it)
      it->second->setReadyBits();
  }
  /* Reset all the ready bits in all the scratchpads. */
  void resetReadyBits() {
    for (auto it = logical_arrays.begin(); it != logical_arrays.end(); ++it)
      it->second->resetReadyBits();
  }

  /* Increment the loads counter for the specified partition. */
  void increment_loads(std::string array_label, unsigned index);
  /* Increment the stores counter for the specified partition. */
  void increment_stores(std::string array_label, unsigned index);

  /* For accesses that are caused by DMA operations, increment the loads
     counter by num_accesses for the specified partition. */
  void increment_dma_loads(std::string base_part, unsigned num_accesses);
  /* For accesses that are caused by DMA operations, increment the stores
     counter by num_accesses for the specified partition. */
  void increment_dma_stores(std::string base_part, unsigned num_accesses);

  void getMemoryBlocks(std::vector<std::string>& names);

  void getAveragePower(unsigned int cycles,
                       float* avg_power,
                       float* avg_dynamic,
                       float* avg_leakage);
  float getTotalArea();

 private:
  /* Num of read/write ports per partition. */
  unsigned num_ports;
  /* Set if ReadyPartition is used. */
  bool ready_mode;
  float cycleTime;  // in ns
  std::unordered_map<std::string, LogicalArray*> logical_arrays;
};

#endif
