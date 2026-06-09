#ifndef __GENERATORS_HAZARD_DETECTION_HAZARD_DETECTION_HH__
#define __GENERATORS_HAZARD_DETECTION_HAZARD_DETECTION_HH__

#include <cstdint>

#include "params/HazardDetection.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class HazardDetectionParams;

class HazardDetection : public SimObject {
private:
    // Input registers
    uint8_t id_rs1_reg;
    uint8_t id_rs2_reg;
    uint8_t ex_rd_reg;
    uint8_t wb_rd_reg;
    uint8_t ex_reg_write_reg;
    uint8_t wb_reg_write_reg;
    uint8_t ex_mem_read_reg;
    uint8_t is_branch_reg;
    uint8_t is_jump_reg;
    uint8_t branch_taken_reg;
    uint8_t div_busy_reg;
    uint8_t mem_busy_reg;
    uint8_t inst_not_ready_reg;

    // Output registers
    uint8_t hold_flag_reg;
    uint8_t flush_if_id_reg;
    uint8_t flush_id_ex_reg;
    uint8_t forward_a_sel_reg;
    uint8_t forward_b_sel_reg;
    uint8_t can_forward_a_reg;
    uint8_t can_forward_b_reg;

public:
    HazardDetection(const HazardDetectionParams &params);

    // Input port set functions
    void setIdRs1(uint8_t val);
    void setIdRs2(uint8_t val);
    void setExRd(uint8_t val);
    void setWbRd(uint8_t val);
    void setExRegWrite(uint8_t val);
    void setWbRegWrite(uint8_t val);
    void setExMemRead(uint8_t val);
    void setIsBranch(uint8_t val);
    void setIsJump(uint8_t val);
    void setBranchTaken(uint8_t val);
    void setDivBusy(uint8_t val);
    void setMemBusy(uint8_t val);
    void setInstNotReady(uint8_t val);

    // Output port get functions
    uint8_t getHoldFlag();
    uint8_t getFlushIfId();
    uint8_t getFlushIdEx();
    uint8_t getForwardASel();
    uint8_t getForwardBSel();
    uint8_t getCanForwardA();
    uint8_t getCanForwardB();

    // Process function (combinational logic)
    void process();
};

} // namespace gem5

#endif // __GENERATORS_HAZARD_DETECTION_HAZARD_DETECTION_HH__
