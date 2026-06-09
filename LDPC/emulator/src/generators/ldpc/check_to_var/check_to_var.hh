#ifndef __GENERATORS_LDPC_CHECK_TO_VAR_HH__
#define __GENERATORS_LDPC_CHECK_TO_VAR_HH__

#include <cstdint>

#include "params/CheckToVar.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class CheckToVarParams;

class CheckToVar : public SimObject {
  private:
    uint64_t neighbour_vars_low_reg;
    uint64_t neighbour_vars_high_reg;

    int32_t check_to_var_message_val;
    int32_t c2v_msg[4];

  public:
    CheckToVar(const CheckToVarParams &params);

    void setNeighbourVarsLow(uint64_t val);
    void setNeighbourVarsHigh(uint64_t val);

    int32_t getCheckToVarMessage();
    int32_t getC2VOutput(int idx);

    void process();
};

} // namespace gem5

#endif // __GENERATORS_LDPC_CHECK_TO_VAR_HH__
