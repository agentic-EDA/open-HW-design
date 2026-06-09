#ifndef __GENERATORS_BRANCH_LOGIC_BRANCH_LOGIC_HH__
#define __GENERATORS_BRANCH_LOGIC_BRANCH_LOGIC_HH__

#include <cstdint>

#include "params/BranchLogic.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class BranchLogicParams;

class BranchLogic : public SimObject {
  private:
    uint32_t rs1_val_reg;
    uint32_t rs2_val_reg;
    uint32_t branch_op_reg;
    uint32_t branch_imm_reg;
    uint32_t current_pc_reg;

    uint32_t branch_taken_reg;
    uint32_t branch_target_reg;

  public:
    BranchLogic(const BranchLogicParams &params);

    void setRs1Val(uint32_t val);
    void setRs2Val(uint32_t val);
    void setBranchOp(uint32_t val);
    void setBranchImm(uint32_t val);
    void setCurrentPc(uint32_t val);

    uint32_t getBranchTaken();
    uint32_t getBranchTarget();

    void process();
};

} // namespace gem5

#endif // __GENERATORS_BRANCH_LOGIC_BRANCH_LOGIC_HH__
