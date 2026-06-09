#ifndef __GENERATORS_RISCV_CORE_RISCV_CORE_HH__
#define __GENERATORS_RISCV_CORE_RISCV_CORE_HH__

#include <cstdint>
#include <cassert>

#include "params/RiscvCore.hh"
#include "sim/sim_object.hh"
#include "sim/eventq.hh"

#include "generators/riscv/alu/alu.hh"
#include "generators/riscv/branch_logic/branch_logic.hh"
#include "generators/riscv/forwarding_unit/forwarding_unit.hh"
#include "generators/riscv/hazard_detection/hazard_detection.hh"
#include "generators/riscv/instruction_decoder/instruction_decoder.hh"
#include "generators/riscv/pc_logic/pc_logic.hh"

namespace gem5 {

class RiscvCore : public SimObject {
private:
    // Clock and reset inputs
    uint8_t rst_i_reg;
    
    // Instruction bus inputs
    uint32_t rib_inst_i_reg;
    uint8_t rib_inst_ready_i_reg;
    
    // Data bus inputs
    uint32_t rib_data_i_reg;
    uint8_t rib_data_ready_i_reg;
    uint8_t rib_data_ack_i_reg;
    
    // CLINT interrupt inputs
    uint8_t clint_int_i_reg;
    uint8_t clint_int_assert_i_reg;
    
    // Division unit inputs
    uint32_t div_result_i_reg;
    uint8_t div_ready_i_reg;
    
    // Output signals
    uint32_t rib_inst_addr_o_val;
    uint8_t rib_inst_req_o_val;
    uint8_t rib_inst_en_o_val;
    uint32_t rib_data_addr_o_val;
    uint32_t rib_data_wdata_o_val;
    uint8_t rib_data_we_o_val;
    uint8_t rib_data_re_o_val;
    uint8_t rib_data_en_o_val;
    uint8_t rib_data_size_o_val;
    uint8_t rib_data_req_o_val;
    uint8_t clint_int_o_val;
    uint8_t clint_int_assert_o_val;
    
    // Sub-module instances
    Alu &alu;
    BranchLogic &branch_logic;
    ForwardingUnit &forwarding_unit;
    HazardDetection &hazard_detection;
    InstructionDecoder &instruction_decoder;
    PcLogic &pc_logic;
    
    // Clock period
    const Tick clock_period;
    
    // Reset address
    const uint32_t reset_addr;
    
    // Interrupt entry address
    const uint32_t int_timer0_entry_addr;
    
    // Pipeline state machine (2 bits)
    uint8_t pipeline_state;
    static const uint8_t S_RUN = 0;
    static const uint8_t S_HOLD = 1;
    static const uint8_t S_STALL = 2;
    static const uint8_t S_EXCEPTION = 3;
    
    // Interrupt state machine (4 bits)
    uint8_t int_state;
    static const uint8_t S_INT_IDLE = 1;
    static const uint8_t S_INT_SYNC_ASSERT = 2;
    static const uint8_t S_INT_ASYNC_ASSERT = 4;
    static const uint8_t S_INT_MRET = 8;
    
    // PC register
    uint32_t pc_reg;
    
    // IF/ID pipeline register
    uint8_t if_id_valid;
    uint32_t if_id_pc;
    uint32_t if_id_inst;
    
    // ID/EX pipeline register
    uint8_t id_ex_valid;
    uint32_t id_ex_pc;
    uint32_t id_ex_pc_plus4;
    uint32_t id_ex_rs1_val;
    uint32_t id_ex_rs2_val;
    uint32_t id_ex_imm;
    uint8_t id_ex_rd;
    uint8_t id_ex_rs1;
    uint8_t id_ex_rs2;
    uint8_t id_ex_alu_op;
    uint8_t id_ex_mem_op;
    uint8_t id_ex_reg_write;
    uint8_t id_ex_mem_read;
    uint8_t id_ex_mem_write;
    uint8_t id_ex_is_branch;
    uint8_t id_ex_branch_op;
    uint8_t id_ex_is_jal;
    uint8_t id_ex_is_jalr;
    uint8_t id_ex_is_alu_reg;
    uint8_t id_ex_is_alu_imm;
    uint8_t id_ex_is_lui;
    uint8_t id_ex_is_auipc;
    uint8_t id_ex_is_load;
    uint8_t id_ex_is_store;
    uint8_t id_ex_is_csr;
    uint8_t id_ex_csr_op;
    uint8_t id_ex_csr_use_imm;
    uint8_t id_ex_is_mul;
    uint8_t id_ex_is_div;
    uint8_t id_ex_is_mret;
    uint8_t id_ex_is_ecall;
    uint8_t id_ex_is_ebreak;
    uint8_t id_ex_is_nop;
    uint8_t id_ex_is_fence;
    
    // Register file (32 x 32-bit)
    uint32_t regs[32];
    
    // CSR registers
    uint32_t csr_mstatus;
    uint32_t csr_mie;
    uint32_t csr_mip;
    uint32_t csr_mtvec;
    uint32_t csr_mscratch;
    uint32_t csr_mepc;
    uint32_t csr_mcause;
    uint64_t csr_mcycle;
    uint64_t csr_minstret;
    
    // Division internal state
    uint8_t div_busy;
    uint8_t div_done;        // Set when division computation finishes
    uint8_t div_op;          // 0: DIV, 1: DIVU, 2: REM, 3: REMU
    uint32_t div_dividend;
    uint32_t div_divisor;
    uint32_t div_remainder;
    uint32_t div_quotient;
    uint8_t div_sign;
    int div_count;
    
    // Hazard control signals
    uint8_t hold_flag;
    uint8_t flush_if_id;
    uint8_t flush_id_ex;
    uint8_t hold_pc;
    uint8_t hold_if;
    uint8_t hold_id;
    
    // Branch result registers (needed for pc_logic input)
    uint8_t branch_taken_reg;
    uint32_t branch_target_reg;
    
    // EX stage result for forwarding
    uint32_t ex_alu_result;
    uint8_t ex_reg_write;
    uint8_t ex_rd;
    
    // Event for clock triggering
    EventFunctionWrapper event;
    
    // Private methods
    void processEvent();
    
    // Helper functions
    uint32_t readReg(uint8_t addr);
    void writeReg(uint8_t addr, uint32_t data);
    uint32_t readCsr(uint32_t addr);
    void writeCsr(uint32_t addr, uint32_t data);
    void updateDivision();
    void startDivision(uint32_t dividend, uint32_t divisor, uint8_t op);
    uint32_t completeDivision();
    
    // One cycle of pipeline operation
    void cycle();
    
    // Execute stage processing (returns next PC)
    uint32_t executeStage(uint32_t &alu_result, uint8_t &reg_write, uint8_t &rd,
                          uint8_t &branch_taken, uint32_t &branch_target);
    
public:
    RiscvCore(const RiscvCoreParams &params);
    
    // Startup function for scheduling first event
    void startup() override;

    // Run one pipeline cycle directly (for testing)
    void runCycle();

    // Input set functions
    void setClk(uint8_t val) {} // Not used, event driven
    void setRst(uint8_t val);
    void setRibInstI(uint32_t val);
    void setRibInstReadyI(uint8_t val);
    void setRibDataI(uint32_t val);
    void setRibDataReadyI(uint8_t val);
    void setRibDataAckI(uint8_t val);
    void setClintIntI(uint8_t val);
    void setClintIntAssertI(uint8_t val);
    void setDivResultI(uint32_t val);
    void setDivReadyI(uint8_t val);
    
    // Output get functions
    uint32_t getRibInstAddrO();
    uint8_t getRibInstReqO();
    uint8_t getRibInstEnO();
    uint32_t getRibDataAddrO();
    uint32_t getRibDataWdataO();
    uint8_t getRibDataWeO();
    uint8_t getRibDataReO();
    uint8_t getRibDataEnO();
    uint8_t getRibDataSizeO();
    uint8_t getRibDataReqO();
    uint8_t getClintIntO();
    uint8_t getClintIntAssertO();
};

} // namespace gem5

#endif // __GENERATORS_RISCV_CORE_RISCV_CORE_HH__
