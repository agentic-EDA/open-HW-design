#include "generators/ldpc/belief/belief.hh"

#include "base/trace.hh"
#include "debug/Belief.hh"

namespace gem5 {

Belief::Belief(const BeliefParams &params) : SimObject(params)
{
    neighbour_checks_low_reg = 0;
    neighbour_checks_high_reg = 0;
    belief_val = 0;
    corrected_bit_val = 0;

    DPRINTF(Belief, "Belief created (combinational)\n");
}

void
Belief::setNeighbourChecksLow(uint64_t val)
{
    neighbour_checks_low_reg = val;
    process();
}

void
Belief::setNeighbourChecksHigh(uint64_t val)
{
    neighbour_checks_high_reg = val;
    process();
}

int32_t
Belief::getBelief()
{
    return belief_val;
}

uint32_t
Belief::getCorrectedBit()
{
    return corrected_bit_val;
}

void
Belief::process()
{
    int32_t elem0 = static_cast<int32_t>(neighbour_checks_low_reg & 0xFFFFFFFF);
    int32_t elem1 = static_cast<int32_t>((neighbour_checks_low_reg >> 32) & 0xFFFFFFFF);
    int32_t elem2 = static_cast<int32_t>(neighbour_checks_high_reg & 0xFFFFFFFF);
    int32_t elem3 = static_cast<int32_t>((neighbour_checks_high_reg >> 32) & 0xFFFFFFFF);

    belief_val = elem0 + elem1 + elem2 + elem3;

    if (belief_val > 0) {
        corrected_bit_val = 0;
    } else {
        corrected_bit_val = 1;
    }
}

} // namespace gem5
