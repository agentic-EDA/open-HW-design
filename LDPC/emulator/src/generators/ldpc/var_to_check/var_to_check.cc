#include "generators/ldpc/var_to_check/var_to_check.hh"

#include "base/trace.hh"
#include "debug/VarToCheck.hh"

namespace gem5 {

VarToCheck::VarToCheck(const VarToCheckParams &params) : SimObject(params)
{
    neighbour_checks_low_reg = 0;
    neighbour_checks_high_reg = 0;
    var_to_check_message_val = 0;

    DPRINTF(VarToCheck, "VarToCheck created (combinational)\n");
}

void
VarToCheck::setNeighbourChecksLow(uint64_t val)
{
    neighbour_checks_low_reg = val;
    process();
}

void
VarToCheck::setNeighbourChecksHigh(uint64_t val)
{
    neighbour_checks_high_reg = val;
    process();
}

int32_t
VarToCheck::getVarToCheckMessage()
{
    return var_to_check_message_val;
}

void
VarToCheck::process()
{
    int32_t elem0 = static_cast<int32_t>(neighbour_checks_low_reg & 0xFFFFFFFF);
    int32_t elem1 = static_cast<int32_t>((neighbour_checks_low_reg >> 32) & 0xFFFFFFFF);
    int32_t elem2 = static_cast<int32_t>(neighbour_checks_high_reg & 0xFFFFFFFF);
    int32_t elem3 = static_cast<int32_t>((neighbour_checks_high_reg >> 32) & 0xFFFFFFFF);

    var_to_check_message_val = elem0 + elem1 + elem2 + elem3;
}

} // namespace gem5
