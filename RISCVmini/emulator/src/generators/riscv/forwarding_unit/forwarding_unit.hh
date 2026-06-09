#ifndef __GENERATORS_FORWARDING_UNIT_FORWARDING_UNIT_HH__
#define __GENERATORS_FORWARDING_UNIT_FORWARDING_UNIT_HH__

#include <cstdint>

#include "params/ForwardingUnit.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class ForwardingUnitParams;

class ForwardingUnit : public SimObject {
private:
    uint8_t id_rs1_reg;
    uint8_t id_rs2_reg;
    uint8_t ex_rd_reg;
    uint8_t mem_rd_reg;
    uint8_t ex_reg_write_reg;
    uint8_t mem_reg_write_reg;
    uint8_t ex_mem_read_reg;

    uint8_t forward_a_sel_reg;
    uint8_t forward_b_sel_reg;

public:
    ForwardingUnit(const ForwardingUnitParams &params);

    void process();

    void setIdRs1(uint8_t val);
    void setIdRs2(uint8_t val);
    void setExRd(uint8_t val);
    void setMemRd(uint8_t val);
    void setExRegWrite(uint8_t val);
    void setMemRegWrite(uint8_t val);
    void setExMemRead(uint8_t val);

    uint8_t getForwardASel();
    uint8_t getForwardBSel();
};

} // namespace gem5

#endif // __GENERATORS_FORWARDING_UNIT_FORWARDING_UNIT_HH__
