#include "generators/riscv/hazard_detection/hazard_detection.hh"

#include "base/trace.hh"
#include "debug/HazardDetection.hh"

namespace gem5 {

// Hold flag encoding
static const uint8_t HOLD_NONE  = 0x0; // 3'b000
static const uint8_t HOLD_PC    = 0x1; // 3'b001
static const uint8_t HOLD_IF    = 0x2; // 3'b010
static const uint8_t HOLD_ID    = 0x3; // 3'b011

// Forward selection encoding
static const uint8_t FWD_NONE   = 0x0; // 2'b00
static const uint8_t FWD_EX     = 0x1; // 2'b01
static const uint8_t FWD_MEM    = 0x2; // 2'b10
static const uint8_t FWD_WB     = 0x3; // 2'b11

HazardDetection::HazardDetection(const HazardDetectionParams &params) :
    SimObject(params),
    id_rs1_reg(0),
    id_rs2_reg(0),
    ex_rd_reg(0),
    wb_rd_reg(0),
    ex_reg_write_reg(0),
    wb_reg_write_reg(0),
    ex_mem_read_reg(0),
    is_branch_reg(0),
    is_jump_reg(0),
    branch_taken_reg(0),
    div_busy_reg(0),
    mem_busy_reg(0),
    inst_not_ready_reg(0),
    hold_flag_reg(HOLD_NONE),
    flush_if_id_reg(0),
    flush_id_ex_reg(0),
    forward_a_sel_reg(FWD_NONE),
    forward_b_sel_reg(FWD_NONE),
    can_forward_a_reg(0),
    can_forward_b_reg(0)
{
    DPRINTF(HazardDetection, "HazardDetection object created\n");
}

void
HazardDetection::setIdRs1(uint8_t val)
{
    id_rs1_reg = val & 0x1F;
}

void
HazardDetection::setIdRs2(uint8_t val)
{
    id_rs2_reg = val & 0x1F;
}

void
HazardDetection::setExRd(uint8_t val)
{
    ex_rd_reg = val & 0x1F;
}

void
HazardDetection::setWbRd(uint8_t val)
{
    wb_rd_reg = val & 0x1F;
}

void
HazardDetection::setExRegWrite(uint8_t val)
{
    ex_reg_write_reg = val & 1;
}

void
HazardDetection::setWbRegWrite(uint8_t val)
{
    wb_reg_write_reg = val & 1;
}

void
HazardDetection::setExMemRead(uint8_t val)
{
    ex_mem_read_reg = val & 1;
}

void
HazardDetection::setIsBranch(uint8_t val)
{
    is_branch_reg = val & 1;
}

void
HazardDetection::setIsJump(uint8_t val)
{
    is_jump_reg = val & 1;
}

void
HazardDetection::setBranchTaken(uint8_t val)
{
    branch_taken_reg = val & 1;
}

void
HazardDetection::setDivBusy(uint8_t val)
{
    div_busy_reg = val & 1;
}

void
HazardDetection::setMemBusy(uint8_t val)
{
    mem_busy_reg = val & 1;
}

void
HazardDetection::setInstNotReady(uint8_t val)
{
    inst_not_ready_reg = val & 1;
}

uint8_t
HazardDetection::getHoldFlag()
{
    return hold_flag_reg;
}

uint8_t
HazardDetection::getFlushIfId()
{
    return flush_if_id_reg;
}

uint8_t
HazardDetection::getFlushIdEx()
{
    return flush_id_ex_reg;
}

uint8_t
HazardDetection::getForwardASel()
{
    return forward_a_sel_reg;
}

uint8_t
HazardDetection::getForwardBSel()
{
    return forward_b_sel_reg;
}

uint8_t
HazardDetection::getCanForwardA()
{
    return can_forward_a_reg;
}

uint8_t
HazardDetection::getCanForwardB()
{
    return can_forward_b_reg;
}

void
HazardDetection::process()
{
    // Reset all outputs to defaults
    hold_flag_reg = HOLD_NONE;
    flush_if_id_reg = 0;
    flush_id_ex_reg = 0;
    forward_a_sel_reg = FWD_NONE;
    forward_b_sel_reg = FWD_NONE;
    can_forward_a_reg = 0;
    can_forward_b_reg = 0;

    // Priority 1 (highest): Structural hazards
    if (div_busy_reg) {
        hold_flag_reg = HOLD_ID;
        DPRINTF(HazardDetection, "Structural hazard: divider busy, hold_id\n");
    } else if (mem_busy_reg) {
        hold_flag_reg = HOLD_IF;
        DPRINTF(HazardDetection, "Structural hazard: memory busy, hold_if\n");
    } else if (inst_not_ready_reg) {
        hold_flag_reg = HOLD_PC;
        DPRINTF(HazardDetection, "Structural hazard: inst not ready, hold_pc\n");
    }
    // Priority 2: Load-Use data hazard (must stall)
    else if (ex_mem_read_reg && (ex_rd_reg != 0) &&
             ((ex_rd_reg == id_rs1_reg) || (ex_rd_reg == id_rs2_reg))) {
        hold_flag_reg = HOLD_ID;
        flush_id_ex_reg = 1;
        DPRINTF(HazardDetection, "Load-Use hazard detected, hold_id + flush_id_ex\n");
    }
    // Priority 3: Control hazards (flush pipeline)
    else if (is_jump_reg) {
        flush_if_id_reg = 1;
        DPRINTF(HazardDetection, "Jump detected, flush_if_id\n");
    } else if (is_branch_reg && branch_taken_reg) {
        flush_if_id_reg = 1;
        DPRINTF(HazardDetection, "Branch taken, flush_if_id\n");
    }

    // Priority 4 (lowest): Forwarding for RAW hazards
    // Forwarding for operand A (rs1)
    if (ex_reg_write_reg && (ex_rd_reg != 0) && (ex_rd_reg == id_rs1_reg)) {
        forward_a_sel_reg = FWD_EX;
        can_forward_a_reg = 1;
        DPRINTF(HazardDetection, "Forward A from EX: rd=%d, rs1=%d\n", ex_rd_reg, id_rs1_reg);
    } else if (wb_reg_write_reg && (wb_rd_reg != 0) && (wb_rd_reg == id_rs1_reg)) {
        forward_a_sel_reg = FWD_MEM;
        can_forward_a_reg = 1;
        DPRINTF(HazardDetection, "Forward A from MEM: rd=%d, rs1=%d\n", wb_rd_reg, id_rs1_reg);
    }

    // Forwarding for operand B (rs2)
    if (ex_reg_write_reg && (ex_rd_reg != 0) && (ex_rd_reg == id_rs2_reg)) {
        forward_b_sel_reg = FWD_EX;
        can_forward_b_reg = 1;
        DPRINTF(HazardDetection, "Forward B from EX: rd=%d, rs2=%d\n", ex_rd_reg, id_rs2_reg);
    } else if (wb_reg_write_reg && (wb_rd_reg != 0) && (wb_rd_reg == id_rs2_reg)) {
        forward_b_sel_reg = FWD_MEM;
        can_forward_b_reg = 1;
        DPRINTF(HazardDetection, "Forward B from MEM: rd=%d, rs2=%d\n", wb_rd_reg, id_rs2_reg);
    }
}

} // namespace gem5
