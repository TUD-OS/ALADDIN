#ifndef __LOGICAL_ARRAY__
#define __LOGICAL_ARRAY__

#include "Partition.h"
#include "ReadyPartition.h"

/* A logical array class for each individual array. The logical array stores
 * the basic information of each array, like array name or partition type. It
 * also includes the physical partitions of each array.*/

class LogicalArray {
 public:
  LogicalArray(std::string _base_name,
                uint64_t _base_addr,
                PartitionType _partition_type,
                unsigned _partition_factor,
                unsigned _total_size,
                unsigned _word_size,
                unsigned _num_ports,
                bool _ready_mode);
  ~LogicalArray();
  void step();
  /* Return true if any Partition can service. */
  bool canService();
  /* Return true if the partition with index part_index can service. */
  bool canService(unsigned part_index, uint64_t addr, bool isLoad);

  /* Setters. */
  void setWordSize(unsigned _word_size) { word_size = _word_size; }
  void setReadEnergy(float _read_energy) { part_read_energy = _read_energy; }
  void setWriteEnergy(float _write_energy) {
    part_write_energy = _write_energy;
  }
  void setLeakagePower(float _leak_power) { part_leak_power = _leak_power; }
  void setArea(float _area) { part_area = _area; }
  /* Accessors. */
  /* Find the data block index for address addr in partition part_index. */
  std::string getBaseName() { return base_name; }
  unsigned getBlockIndex(unsigned part_index, uint64_t addr);

  unsigned getTotalLoads();
  unsigned getTotalStores();

  float getReadEnergy() { return getTotalLoads() * part_read_energy; }
  float getWriteEnergy() { return getTotalStores() * part_write_energy; }
  float getLeakagePower() { return part_leak_power * num_partitions; }
  float getArea() { return part_area * num_partitions; }

  /* Increment loads/stores for each Partition. */
  void increment_loads(unsigned part_index);
  void increment_stores(unsigned part_index);

  void increment_loads(unsigned part_index, unsigned num_accesses);
  void increment_stores(unsigned part_index, unsigned num_accesses);

  /* Increment loads/stores for all the Partitions inside the LogicalArray. */
  void increment_streaming_loads(unsigned streaming_size);
  void increment_streaming_stores(unsigned streaming_size);

  /* Ready bit handling. */
  void setReadyBit(unsigned part_index, uint64_t addr);
  void resetReadyBit(unsigned part_index, uint64_t addr);
  void setReadyBits(unsigned part_index) {
    partitions[part_index]->setAllReadyBits();
  }
  void resetReadyBits(unsigned part_index) {
    partitions[part_index]->resetAllReadyBits();
  }
  void setReadyBits() {
    for ( Partition* part : partitions)
      part->setAllReadyBits();
  }
  void resetReadyBits() {
    for ( Partition* part : partitions)
      part->resetAllReadyBits();
  }
 protected:
  /* Array label for the LogicalArray. */
  std::string base_name;
  /* Base address for the LogicalArray. */
  uint64_t base_addr;
  /* Partition types: block or cyclic. */
  PartitionType partition_type;
  /* Num of partitions inside this LogicalArray. */
  unsigned num_partitions;
  /* Total size in Bytes for all the partitions. */
  unsigned total_size;
  /* Word size for each partition. */
  unsigned word_size;
  /* Num of ports for each partition. */
  unsigned num_ports;
  /* All the Partitions inside the same LogicalArray have the same
   * energy/power/area characteristics. */
  /* Per access read energy for each partition. */
  float part_read_energy;
  /* Per access write  energy for each partition. */
  float part_write_energy;
  /* Leakage power for each partition. */
  float part_leak_power;
  /* Area for each partititon. */
  float part_area;
  /* A list of Partitions that are part of the LogicalArray. */
  std::vector<Partition*> partitions;
};

#endif
