#include "generators/riscv/pc_logic/pc_logic.hh"

#include "base/trace.hh"
#include "debug/PcLogic.hh"

namespace gem5 {

PcLogic::PcLogic(const PcLogicParams &params) : SimObject(params) {
    current_pc_reg = 0;
    pc_plus4_reg = 0;
    branch_target_reg = 0;
    jump_target_reg = 0;
    jalr_target_reg = 0;
    is_jal_reg = 0;
    is_jalr_reg = 0;
    branch_taken_reg = 0;
    is_exception_reg = 0;
    exception_addr_reg = 0;
    is_mret_reg = 0;
    mret_addr_reg = 0;
    hold_pc_reg = 0;
    rst_reg = 1;

    next_pc_val = 0;
    pc_sel_val = 0;

    DPRINTF(PcLogic, "PcLogic object created (combinational)\n");
}

void
PcLogic::compute()
{
    if (!rst_reg) {
        next_pc_val = CpuResetAddr;
        pc_sel_val = 0;
        return;
    }

    if (is_exception_reg) {
        next_pc_val = exception_addr_reg;
        pc_sel_val = 3;
        return;
    }

    if (is_mret_reg) {
        next_pc_val = mret_addr_reg;
        pc_sel_val = 3;
        return;
    }

    if (hold_pc_reg) {
        next_pc_val = current_pc_reg;
        pc_sel_val = 0;
        return;
    }

    if (is_jalr_reg) {
        next_pc_val = jalr_target_reg;
        pc_sel_val = 2;
        return;
    }

    if (is_jal_reg) {
        next_pc_val = jump_target_reg;
        pc_sel_val = 2;
        return;
    }

    if (branch_taken_reg) {
        next_pc_val = branch_target_reg;
        pc_sel_val = 1;
        return;
    }

    next_pc_val = pc_plus4_reg;
    pc_sel_val = 0;
}

void
PcLogic::process()
{
    compute();
}

void
PcLogic::setCurrentPc(uint32_t val)
{
    current_pc_reg = val;
    compute();
}

void
PcLogic::setPcPlus4(uint32_t val)
{
    pc_plus4_reg = val;
    compute();
}

void
PcLogic::setBranchTarget(uint32_t val)
{
    branch_target_reg = val;
    compute();
}

void
PcLogic::setJumpTarget(uint32_t val)
{
    jump_target_reg = val;
    compute();
}

void
PcLogic::setJalrTarget(uint32_t val)
{
    jalr_target_reg = val;
    compute();
}

void
PcLogic::setIsJal(uint8_t val)
{
    is_jal_reg = val & 1;
    compute();
}

void
PcLogic::setIsJalr(uint8_t val)
{
    is_jalr_reg = val & 1;
    compute();
}

void
PcLogic::setBranchTaken(uint8_t val)
{
    branch_taken_reg = val & 1;
    compute();
}

void
PcLogic::setIsException(uint8_t val)
{
    is_exception_reg = val & 1;
    compute();
}

void
PcLogic::setExceptionAddr(uint32_t val)
{
    exception_addr_reg = val;
    compute();
}

void
PcLogic::setIsMret(uint8_t val)
{
    is_mret_reg = val & 1;
    compute();
}

void
PcLogic::setMretAddr(uint32_t val)
{
    mret_addr_reg = val;
    compute();
}

void
PcLogic::setHoldPc(uint8_t val)
{
    hold_pc_reg = val & 1;
    compute();
}

void
PcLogic::setRst(uint8_t val)
{
    rst_reg = val & 1;
    compute();
}

uint32_t
PcLogic::getNextPc()
{
    return next_pc_val;
}

uint8_t
PcLogic::getPcSel()
{
    return pc_sel_val;
}

} // namespace gem5
