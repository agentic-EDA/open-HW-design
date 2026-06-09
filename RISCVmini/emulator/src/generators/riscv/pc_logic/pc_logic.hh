#ifndef __GENERATORS_PC_LOGIC_PC_LOGIC_HH__
#define __GENERATORS_PC_LOGIC_PC_LOGIC_HH__

#include <cstdint>

#include "params/PcLogic.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class PcLogicParams;

class PcLogic : public SimObject {
private:
    static const uint32_t CpuResetAddr = 0x00000000;

    uint32_t current_pc_reg;
    uint32_t pc_plus4_reg;
    uint32_t branch_target_reg;
    uint32_t jump_target_reg;
    uint32_t jalr_target_reg;
    uint8_t is_jal_reg;
    uint8_t is_jalr_reg;
    uint8_t branch_taken_reg;
    uint8_t is_exception_reg;
    uint32_t exception_addr_reg;
    uint8_t is_mret_reg;
    uint32_t mret_addr_reg;
    uint8_t hold_pc_reg;
    uint8_t rst_reg;

    uint32_t next_pc_val;
    uint8_t pc_sel_val;

    void compute();

public:
    PcLogic(const PcLogicParams &params);

    void setCurrentPc(uint32_t val);
    void setPcPlus4(uint32_t val);
    void setBranchTarget(uint32_t val);
    void setJumpTarget(uint32_t val);
    void setJalrTarget(uint32_t val);
    void setIsJal(uint8_t val);
    void setIsJalr(uint8_t val);
    void setBranchTaken(uint8_t val);
    void setIsException(uint8_t val);
    void setExceptionAddr(uint32_t val);
    void setIsMret(uint8_t val);
    void setMretAddr(uint32_t val);
    void setHoldPc(uint8_t val);
    void setRst(uint8_t val);

    uint32_t getNextPc();
    uint8_t getPcSel();

    void process();
};

} // namespace gem5

#endif // __GENERATORS_PC_LOGIC_PC_LOGIC_HH__
