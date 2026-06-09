#ifndef __GENERATORS_LDPC_VAR_TO_CHECK_HH__
#define __GENERATORS_LDPC_VAR_TO_CHECK_HH__

#include <cstdint>

#include "params/VarToCheck.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class VarToCheckParams;

class VarToCheck : public SimObject {
  private:
    uint64_t neighbour_checks_low_reg;
    uint64_t neighbour_checks_high_reg;

    int32_t var_to_check_message_val;

  public:
    VarToCheck(const VarToCheckParams &params);

    void setNeighbourChecksLow(uint64_t val);
    void setNeighbourChecksHigh(uint64_t val);

    int32_t getVarToCheckMessage();

    void process();
};

} // namespace gem5

#endif // __GENERATORS_LDPC_VAR_TO_CHECK_HH__
