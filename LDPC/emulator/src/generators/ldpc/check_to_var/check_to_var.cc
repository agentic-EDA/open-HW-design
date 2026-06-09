#include "generators/ldpc/check_to_var/check_to_var.hh"

#include <cstdlib>

#include "base/trace.hh"
#include "debug/CheckToVar.hh"

namespace gem5 {

CheckToVar::CheckToVar(const CheckToVarParams &params) : SimObject(params)
{
    neighbour_vars_low_reg = 0;
    neighbour_vars_high_reg = 0;
    check_to_var_message_val = 0;
    for (int i = 0; i < 4; i++) c2v_msg[i] = 0;

    DPRINTF(CheckToVar, "CheckToVar created (combinational)\n");
}

void
CheckToVar::setNeighbourVarsLow(uint64_t val)
{
    neighbour_vars_low_reg = val;
    process();
}

void
CheckToVar::setNeighbourVarsHigh(uint64_t val)
{
    neighbour_vars_high_reg = val;
    process();
}

int32_t
CheckToVar::getCheckToVarMessage()
{
    return check_to_var_message_val;
}

int32_t
CheckToVar::getC2VOutput(int idx)
{
    if (idx < 0 || idx >= 4) return 0;
    return c2v_msg[idx];
}

static void
minSumExcluding(int32_t elem[4], int exclude, int32_t& result)
{
    int32_t min_abs = -1;
    int sign = 0;
    bool first = true;
    for (int i = 0; i < 4; i++) {
        if (i == exclude) continue;
        int32_t s = (elem[i] >> 31) & 1;
        int32_t a = std::abs(elem[i]);
        if (first) {
            min_abs = a;
            sign = s;
            first = false;
        } else {
            sign ^= s;
            if (a < min_abs) min_abs = a;
        }
    }
    result = sign ? -min_abs : min_abs;
}

void
CheckToVar::process()
{
    int32_t elem0 = static_cast<int32_t>(neighbour_vars_low_reg & 0xFFFFFFFF);
    int32_t elem1 = static_cast<int32_t>((neighbour_vars_low_reg >> 32) & 0xFFFFFFFF);
    int32_t elem2 = static_cast<int32_t>(neighbour_vars_high_reg & 0xFFFFFFFF);
    int32_t elem3 = static_cast<int32_t>((neighbour_vars_high_reg >> 32) & 0xFFFFFFFF);

    int32_t elem[4] = {elem0, elem1, elem2, elem3};
    int32_t all_min = std::abs(elem0);
    int all_sign = (elem0 >> 31) & 1;
    for (int i = 1; i < 4; i++) {
        all_sign ^= (elem[i] >> 31) & 1;
        int32_t a = std::abs(elem[i]);
        if (a < all_min) all_min = a;
    }
    check_to_var_message_val = all_sign ? -all_min : all_min;

    for (int i = 0; i < 4; i++) {
        minSumExcluding(elem, i, c2v_msg[i]);
    }
}

} // namespace gem5
