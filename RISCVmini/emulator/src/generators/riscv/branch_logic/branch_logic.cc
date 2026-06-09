#include "generators/riscv/branch_logic/branch_logic.hh"

#include "base/trace.hh"
#include "debug/BranchLogic.hh"

namespace gem5 {

BranchLogic::BranchLogic(const BranchLogicParams &params) : SimObject(params)
{
    rs1_val_reg = 0;
    rs2_val_reg = 0;
    branch_op_reg = 0;
    branch_imm_reg = 0;
    current_pc_reg = 0;

    branch_taken_reg = 0;
    branch_target_reg = 0;

    DPRINTF(BranchLogic, "BranchLogic object created\n");
}

void
BranchLogic::setRs1Val(uint32_t val)
{
    rs1_val_reg = val;
}

void
BranchLogic::setRs2Val(uint32_t val)
{
    rs2_val_reg = val;
}

void
BranchLogic::setBranchOp(uint32_t val)
{
    branch_op_reg = val & 0x7;
}

void
BranchLogic::setBranchImm(uint32_t val)
{
    branch_imm_reg = val;
}

void
BranchLogic::setCurrentPc(uint32_t val)
{
    current_pc_reg = val;
}

uint32_t
BranchLogic::getBranchTaken()
{
    return branch_taken_reg;
}

uint32_t
BranchLogic::getBranchTarget()
{
    return branch_target_reg;
}

void
BranchLogic::process()
{
    int32_t rs1_signed = static_cast<int32_t>(rs1_val_reg);
    int32_t rs2_signed = static_cast<int32_t>(rs2_val_reg);

    uint32_t op = branch_op_reg;

    if (op == 0x0) {
        branch_taken_reg = (rs1_val_reg == rs2_val_reg) ? 1 : 0;
    } else if (op == 0x1) {
        branch_taken_reg = (rs1_val_reg != rs2_val_reg) ? 1 : 0;
    } else if (op == 0x4) {
        branch_taken_reg = (rs1_signed < rs2_signed) ? 1 : 0;
    } else if (op == 0x5) {
        branch_taken_reg = (rs1_signed >= rs2_signed) ? 1 : 0;
    } else if (op == 0x6) {
        branch_taken_reg = (rs1_val_reg < rs2_val_reg) ? 1 : 0;
    } else if (op == 0x7) {
        branch_taken_reg = (rs1_val_reg >= rs2_val_reg) ? 1 : 0;
    } else {
        branch_taken_reg = 0;
    }

    branch_target_reg = current_pc_reg + branch_imm_reg;

    DPRINTF(BranchLogic,
            "BranchLogic: op=0x%x, rs1=0x%x, rs2=0x%x, taken=%d, target=0x%x\n",
            op, rs1_val_reg, rs2_val_reg, branch_taken_reg, branch_target_reg);
}

} // namespace gem5
