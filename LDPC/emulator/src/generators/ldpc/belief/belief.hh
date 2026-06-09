#ifndef __GENERATORS_LDPC_BELIEF_HH__
#define __GENERATORS_LDPC_BELIEF_HH__

#include <cstdint>

#include "params/Belief.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class BeliefParams;

class Belief : public SimObject {
  private:
    uint64_t neighbour_checks_low_reg;
    uint64_t neighbour_checks_high_reg;

    int32_t belief_val;
    uint32_t corrected_bit_val;

  public:
    Belief(const BeliefParams &params);

    void setNeighbourChecksLow(uint64_t val);
    void setNeighbourChecksHigh(uint64_t val);

    int32_t getBelief();
    uint32_t getCorrectedBit();

    void process();
};

} // namespace gem5

#endif // __GENERATORS_LDPC_BELIEF_HH__
