#include "generators/riscv/forwarding_unit/forwarding_unit.hh"

#include "base/trace.hh"
#include "debug/ForwardingUnit.hh"

namespace gem5 {

ForwardingUnit::ForwardingUnit(const ForwardingUnitParams &params) :
    SimObject(params),
    id_rs1_reg(0),
    id_rs2_reg(0),
    ex_rd_reg(0),
    mem_rd_reg(0),
    ex_reg_write_reg(0),
    mem_reg_write_reg(0),
    ex_mem_read_reg(0),
    forward_a_sel_reg(0),
    forward_b_sel_reg(0)
{
    DPRINTF(ForwardingUnit, "ForwardingUnit object created\n");
}

void
ForwardingUnit::process()
{
    forward_a_sel_reg = 0;
    forward_b_sel_reg = 0;

    if (ex_reg_write_reg && ex_rd_reg != 0 && ex_rd_reg == id_rs1_reg) {
        forward_a_sel_reg = 1;
    } else if (mem_reg_write_reg && mem_rd_reg != 0 && mem_rd_reg == id_rs1_reg) {
        forward_a_sel_reg = 2;
    }

    if (ex_reg_write_reg && ex_rd_reg != 0 && ex_rd_reg == id_rs2_reg) {
        forward_b_sel_reg = 1;
    } else if (mem_reg_write_reg && mem_rd_reg != 0 && mem_rd_reg == id_rs2_reg) {
        forward_b_sel_reg = 2;
    }

    DPRINTF(ForwardingUnit,
            "Forwarding: id_rs1=%d id_rs2=%d ex_rd=%d mem_rd=%d "
            "ex_reg_write=%d mem_reg_write=%d => forward_a_sel=%d forward_b_sel=%d\n",
            id_rs1_reg, id_rs2_reg, ex_rd_reg, mem_rd_reg,
            ex_reg_write_reg, mem_reg_write_reg,
            forward_a_sel_reg, forward_b_sel_reg);
}

void
ForwardingUnit::setIdRs1(uint8_t val)
{
    id_rs1_reg = val & 0x1F;
}

void
ForwardingUnit::setIdRs2(uint8_t val)
{
    id_rs2_reg = val & 0x1F;
}

void
ForwardingUnit::setExRd(uint8_t val)
{
    ex_rd_reg = val & 0x1F;
}

void
ForwardingUnit::setMemRd(uint8_t val)
{
    mem_rd_reg = val & 0x1F;
}

void
ForwardingUnit::setExRegWrite(uint8_t val)
{
    ex_reg_write_reg = val & 1;
}

void
ForwardingUnit::setMemRegWrite(uint8_t val)
{
    mem_reg_write_reg = val & 1;
}

void
ForwardingUnit::setExMemRead(uint8_t val)
{
    ex_mem_read_reg = val & 1;
}

uint8_t
ForwardingUnit::getForwardASel()
{
    return forward_a_sel_reg;
}

uint8_t
ForwardingUnit::getForwardBSel()
{
    return forward_b_sel_reg;
}

} // namespace gem5
