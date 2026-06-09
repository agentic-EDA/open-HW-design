#include "generators/riscv/riscv_core/riscv_core.hh"

#include "base/trace.hh"
#include "debug/RiscvCore.hh"

namespace gem5 {

RiscvCore::RiscvCore(const RiscvCoreParams &params) :
    SimObject(params),
    alu(*params.alu),
    branch_logic(*params.branch_logic),
    forwarding_unit(*params.forwarding_unit),
    hazard_detection(*params.hazard_detection),
    instruction_decoder(*params.instruction_decoder),
    pc_logic(*params.pc_logic),
    clock_period(params.clock_period),
    reset_addr(params.reset_addr),
    int_timer0_entry_addr(params.int_timer0_entry_addr),
    event([this]{processEvent();}, name()),
    rst_i_reg(1),
    rib_inst_i_reg(0),
    rib_inst_ready_i_reg(0),
    rib_data_i_reg(0),
    rib_data_ready_i_reg(0),
    rib_data_ack_i_reg(0),
    clint_int_i_reg(0),
    clint_int_assert_i_reg(0),
    div_result_i_reg(0),
    div_ready_i_reg(0),
    rib_inst_addr_o_val(0),
    rib_inst_req_o_val(0),
    rib_inst_en_o_val(0),
    rib_data_addr_o_val(0),
    rib_data_wdata_o_val(0),
    rib_data_we_o_val(0),
    rib_data_re_o_val(0),
    rib_data_en_o_val(0),
    rib_data_size_o_val(0),
    rib_data_req_o_val(0),
    clint_int_o_val(0),
    clint_int_assert_o_val(0),
    pipeline_state(S_RUN),
    int_state(S_INT_IDLE),
    pc_reg(reset_addr),
    if_id_valid(0),
    if_id_pc(0),
    if_id_inst(0),
    id_ex_valid(0),
    id_ex_pc(0),
    id_ex_pc_plus4(0),
    id_ex_rs1_val(0),
    id_ex_rs2_val(0),
    id_ex_imm(0),
    id_ex_rd(0),
    id_ex_rs1(0),
    id_ex_rs2(0),
    id_ex_alu_op(0),
    id_ex_mem_op(0),
    id_ex_reg_write(0),
    id_ex_mem_read(0),
    id_ex_mem_write(0),
    id_ex_is_branch(0),
    id_ex_branch_op(0),
    id_ex_is_jal(0),
    id_ex_is_jalr(0),
    id_ex_is_alu_reg(0),
    id_ex_is_alu_imm(0),
    id_ex_is_lui(0),
    id_ex_is_auipc(0),
    id_ex_is_load(0),
    id_ex_is_store(0),
    id_ex_is_csr(0),
    id_ex_csr_op(0),
    id_ex_csr_use_imm(0),
    id_ex_is_mul(0),
    id_ex_is_div(0),
    id_ex_is_mret(0),
    id_ex_is_ecall(0),
    id_ex_is_ebreak(0),
    id_ex_is_nop(0),
    id_ex_is_fence(0),
    div_busy(0),
    div_done(0),
    div_op(0),
    div_dividend(0),
    div_divisor(0),
    div_remainder(0),
    div_quotient(0),
    div_sign(0),
    div_count(0),
    hold_flag(0),
    flush_if_id(0),
    flush_id_ex(0),
    hold_pc(0),
    hold_if(0),
    hold_id(0),
    branch_taken_reg(0),
    branch_target_reg(0),
    ex_alu_result(0),
    ex_reg_write(0),
    ex_rd(0)
{
    for (int i = 0; i < 32; i++) {
        regs[i] = 0;
    }
    
    csr_mstatus = 0;
    csr_mie = 0;
    csr_mip = 0;
    csr_mtvec = 0;
    csr_mscratch = 0;
    csr_mepc = 0;
    csr_mcause = 0;
    csr_mcycle = 0;
    csr_minstret = 0;
    
    DPRINTF(RiscvCore, "RISC-V core created, reset_addr=0x%x\n", reset_addr);
}

void
RiscvCore::startup()
{
    if (!event.scheduled()) {
        schedule(event, clock_period);
    }
    DPRINTF(RiscvCore, "RISC-V core startup, first event scheduled at tick %lu\n", clock_period);
}

void
RiscvCore::runCycle()
{
    processEvent();
}

void
RiscvCore::processEvent()
{
    if (rst_i_reg == 0) {
        pipeline_state = S_RUN;
        int_state = S_INT_IDLE;
        pc_reg = reset_addr;
        if_id_valid = 0;
        id_ex_valid = 0;
        div_busy = 0;
        div_done = 0;
        hold_flag = 0;
        flush_if_id = 0;
        flush_id_ex = 0;
        hold_pc = 0;
        hold_if = 0;
        hold_id = 0;
        
        csr_mstatus = 0;
        csr_mie = 0;
        csr_mip = 0;
        csr_mtvec = 0;
        csr_mepc = 0;
        csr_mcause = 0;
        
        rib_inst_addr_o_val = reset_addr;
        rib_inst_req_o_val = 0;
        rib_inst_en_o_val = 1;
        rib_data_addr_o_val = 0;
        rib_data_wdata_o_val = 0;
        rib_data_we_o_val = 0;
        rib_data_re_o_val = 0;
        rib_data_en_o_val = 0;
        rib_data_size_o_val = 0;
        rib_data_req_o_val = 0;
        clint_int_o_val = 0;
        clint_int_assert_o_val = 0;
        
        DPRINTF(RiscvCore, "Reset asserted, PC set to 0x%x\n", reset_addr);
    } else {
        cycle();
    }
    
    if (!event.scheduled()) {
        schedule(event, curTick() + clock_period);
    }
}

void
RiscvCore::cycle()
{
    uint32_t ex_pc_next = 0;
    bool ex_flush = false;
    
    // Update division unit if busy
    if (div_busy) {
        updateDivision();
    }
    
    // Stage 1: Instruction Fetch (IF)
    if (hold_pc == 0) {
        rib_inst_addr_o_val = pc_reg;
        rib_inst_req_o_val = 1;
        rib_inst_en_o_val = 1;
    }
    
    // Stage 3 (first part): Execute pending ID/EX instruction
    // Must run before ID stage so forwarding result is available
    ex_alu_result = 0;
    ex_reg_write = 0;
    ex_rd = 0;
    ex_pc_next = pc_reg;
    branch_taken_reg = 0;
    branch_target_reg = 0;
    
    if (id_ex_valid) {
        ex_pc_next = executeStage(ex_alu_result, ex_reg_write, ex_rd,
                                   branch_taken_reg, branch_target_reg);
    }
    
    // Stage 2: Instruction Decode (ID)
    if (if_id_valid && hold_if == 0) {
        instruction_decoder.setInstruction(if_id_inst);
        instruction_decoder.process();
        
        uint8_t rs1 = instruction_decoder.getRs1();
        uint8_t rs2 = instruction_decoder.getRs2();
        
        hazard_detection.setIdRs1(rs1);
        hazard_detection.setIdRs2(rs2);
        hazard_detection.setExRd(id_ex_rd);
        hazard_detection.setWbRd(ex_rd);
        hazard_detection.setExRegWrite(id_ex_reg_write);
        hazard_detection.setWbRegWrite(ex_reg_write);
        hazard_detection.setExMemRead(id_ex_mem_read);
        hazard_detection.setIsBranch(instruction_decoder.getIsBranch());
        hazard_detection.setIsJump(instruction_decoder.getIsJType() | instruction_decoder.getIsJalr());
        hazard_detection.setBranchTaken(0);
        hazard_detection.setDivBusy(div_busy);
        hazard_detection.setMemBusy(0);
        hazard_detection.setInstNotReady(rib_inst_ready_i_reg == 0);
        hazard_detection.process();
        
        hold_flag = hazard_detection.getHoldFlag();
        flush_if_id = hazard_detection.getFlushIfId();
        flush_id_ex = hazard_detection.getFlushIdEx();
        hold_pc = (hold_flag > 0) ? 1 : 0;
        hold_if = (hold_flag > 1) ? 1 : 0;
        hold_id = (hold_flag > 2) ? 1 : 0;
        
        forwarding_unit.setIdRs1(rs1);
        forwarding_unit.setIdRs2(rs2);
        forwarding_unit.setExRd(id_ex_rd);
        forwarding_unit.setMemRd(ex_rd);
        forwarding_unit.setExRegWrite(id_ex_reg_write);
        forwarding_unit.setMemRegWrite(ex_reg_write);
        forwarding_unit.setExMemRead(id_ex_mem_read);
        forwarding_unit.process();
        
        uint8_t forward_a_sel = forwarding_unit.getForwardASel();
        uint8_t forward_b_sel = forwarding_unit.getForwardBSel();
        
        uint32_t operand_a = readReg(rs1);
        uint32_t operand_b = readReg(rs2);
        
        if (forward_a_sel == 1) {
            operand_a = ex_alu_result;
        } else if (forward_a_sel == 2 && ex_reg_write) {
            operand_a = ex_alu_result;
        }
        
        if (forward_b_sel == 1) {
            operand_b = ex_alu_result;
        } else if (forward_b_sel == 2 && ex_reg_write) {
            operand_b = ex_alu_result;
        }
        
        if (hold_id == 0) {
            id_ex_valid = 1;
            id_ex_pc = if_id_pc;
            id_ex_pc_plus4 = if_id_pc + 4;
            id_ex_rs1_val = operand_a;
            id_ex_rs2_val = operand_b;
            id_ex_rd = instruction_decoder.getRd();
            id_ex_rs1 = rs1;
            id_ex_rs2 = rs2;
            id_ex_alu_op = instruction_decoder.getAluOp();
            id_ex_mem_op = instruction_decoder.getMemOp();
            id_ex_reg_write = instruction_decoder.getRegWrite();
            id_ex_mem_read = instruction_decoder.getMemRead();
            id_ex_mem_write = instruction_decoder.getMemWrite();
            id_ex_is_branch = instruction_decoder.getIsBranch();
            id_ex_branch_op = instruction_decoder.getBranchOp();
            id_ex_is_alu_reg = instruction_decoder.getIsAluReg();
            id_ex_is_alu_imm = instruction_decoder.getIsAluImm();
            id_ex_is_jal = instruction_decoder.getIsJType();
            id_ex_is_jalr = instruction_decoder.getIsJalr();
            id_ex_is_lui = instruction_decoder.getIsLui();
            id_ex_is_auipc = instruction_decoder.getIsAuipc();
            id_ex_is_load = instruction_decoder.getIsLoad();
            id_ex_is_store = instruction_decoder.getIsStore();
            id_ex_is_csr = instruction_decoder.getIsCsr();
            id_ex_csr_op = instruction_decoder.getCsrOp();
            id_ex_csr_use_imm = instruction_decoder.getCsrUseImm();
            id_ex_is_mul = instruction_decoder.getIsMul();
            id_ex_is_div = instruction_decoder.getIsDiv();
            id_ex_is_mret = instruction_decoder.getIsMret();
            id_ex_is_ecall = instruction_decoder.getIsEcall();
            id_ex_is_ebreak = instruction_decoder.getIsEbreak();
            id_ex_is_nop = instruction_decoder.getIsNop();
            id_ex_is_fence = instruction_decoder.getIsFence();
            
            if (instruction_decoder.getIsIType()) {
                id_ex_imm = instruction_decoder.getImmI();
            } else if (instruction_decoder.getIsSType()) {
                id_ex_imm = instruction_decoder.getImmS();
            } else if (instruction_decoder.getIsBType()) {
                id_ex_imm = instruction_decoder.getImmB();
            } else if (instruction_decoder.getIsUType()) {
                id_ex_imm = instruction_decoder.getImmU();
            } else if (instruction_decoder.getIsJType()) {
                id_ex_imm = instruction_decoder.getImmJ();
            } else {
                id_ex_imm = 0;
            }
        }
    }
    
    // Apply EX stage writeback and PC update
    if (id_ex_valid && !flush_id_ex) {
        if (ex_reg_write && ex_rd != 0) {
            writeReg(ex_rd, ex_alu_result);
        }
        pc_reg = ex_pc_next;
    }
    
    // Clear ID/EX if flushed or after multi-cycle instruction completion
    if (flush_id_ex) {
        id_ex_valid = 0;
    }
    if (div_done && div_ready_i_reg) {
        id_ex_valid = 0;
    }
    
    // Update PC logic with proper jump targets
    uint32_t jump_target = id_ex_pc + id_ex_imm;
    uint32_t jalr_target = (id_ex_rs1_val + id_ex_imm) & ~1;
    uint32_t exception_addr = csr_mtvec & ~0x3;
    
    pc_logic.setCurrentPc(pc_reg);
    pc_logic.setPcPlus4(pc_reg + 4);
    pc_logic.setBranchTarget(branch_target_reg);
    pc_logic.setJumpTarget(jump_target);
    pc_logic.setJalrTarget(jalr_target);
    pc_logic.setIsJal(id_ex_is_jal);
    pc_logic.setIsJalr(id_ex_is_jalr);
    pc_logic.setBranchTaken(branch_taken_reg);
    pc_logic.setIsException(pipeline_state == S_EXCEPTION ? 1 : 0);
    pc_logic.setExceptionAddr(exception_addr);
    pc_logic.setIsMret(id_ex_is_mret);
    pc_logic.setMretAddr(csr_mepc);
    pc_logic.setHoldPc(hold_pc);
    pc_logic.setRst(rst_i_reg);
    pc_logic.process();
    
    // Update IF/ID pipeline register
    if (rib_inst_ready_i_reg && !hold_if) {
        if_id_valid = 1;
        if_id_pc = pc_reg;
        if_id_inst = rib_inst_i_reg;
        if (flush_if_id) {
            if_id_valid = 0;
        }
    }
    
    csr_mcycle++;
    if (if_id_valid) {
        csr_minstret++;
    }
    
    DPRINTF(RiscvCore, "Cycle: PC=0x%x, inst=0x%x, pipeline_state=%d\n", 
            pc_reg, if_id_inst, pipeline_state);
}

uint32_t
RiscvCore::executeStage(uint32_t &alu_result, uint8_t &reg_write, uint8_t &rd,
                         uint8_t &branch_taken, uint32_t &branch_target)
{
    uint32_t next_pc = pc_reg;
    alu_result = 0;
    reg_write = 0;
    rd = 0;
    branch_taken = 0;
    branch_target = 0;
    
    // Handle MRET
    if (id_ex_is_mret) {
        next_pc = csr_mepc;
        csr_mstatus = (csr_mstatus & ~(1 << 3)) | ((csr_mstatus >> 7) & 1) << 3;
        return next_pc;
    }
    
    // Handle ECALL/EBREAK
    if (id_ex_is_ecall || id_ex_is_ebreak) {
        csr_mepc = id_ex_pc;
        csr_mcause = id_ex_is_ecall ? 11 : 3;
        uint32_t old_mstatus = csr_mstatus;
        csr_mstatus = (csr_mstatus & ~(1 << 3)) | (((old_mstatus >> 3) & 1) << 7);
        next_pc = csr_mtvec & ~0x3;
        if ((csr_mtvec & 0x1) == 1) {
            next_pc += (csr_mcause << 2);
        }
        pipeline_state = S_EXCEPTION;
        DPRINTF(RiscvCore, "Exception: PC set to 0x%x, cause=%d\n", next_pc, csr_mcause);
        return next_pc;
    }
    
    // Handle CSR
    if (id_ex_is_csr) {
        uint32_t csr_val = readCsr(id_ex_imm & 0xFFF);
        uint32_t new_val;
        uint32_t rs1_val_or_imm;
        
        if (id_ex_csr_use_imm) {
            rs1_val_or_imm = id_ex_rs1;
        } else {
            rs1_val_or_imm = id_ex_rs1_val;
        }
        
        if (id_ex_csr_op == 1) {
            new_val = rs1_val_or_imm;
        } else if (id_ex_csr_op == 2) {
            new_val = csr_val | rs1_val_or_imm;
        } else {
            new_val = csr_val & ~rs1_val_or_imm;
        }
        
        writeCsr(id_ex_imm & 0xFFF, new_val);
        
        if (id_ex_rd != 0) {
            reg_write = 1;
            rd = id_ex_rd;
            alu_result = csr_val;
        }
        
        return id_ex_pc + 4;
    }
    
    // Handle DIV
    if (id_ex_is_div) {
        if (div_done && div_ready_i_reg) {
            uint32_t div_result = completeDivision();
            if (id_ex_rd != 0) {
                reg_write = 1;
                rd = id_ex_rd;
                alu_result = div_result;
            }
            div_done = 0;
            return id_ex_pc + 4;
        }
        if (!div_busy && !div_done) {
            startDivision(id_ex_rs1_val, id_ex_rs2_val, id_ex_alu_op);
        }
        return id_ex_pc;
    }
    
    // Handle MUL
    if (id_ex_is_mul) {
        uint64_t product = (uint64_t)(int64_t)(int32_t)id_ex_rs1_val * (uint64_t)(int64_t)(int32_t)id_ex_rs2_val;
        alu_result = (uint32_t)product;
        if (id_ex_rd != 0) {
            reg_write = 1;
            rd = id_ex_rd;
        }
        return id_ex_pc + 4;
    }
    
    // Handle ALU operations
    if (id_ex_is_alu_reg || id_ex_is_alu_imm || id_ex_is_load || id_ex_is_store ||
        id_ex_is_lui || id_ex_is_auipc || id_ex_is_branch || id_ex_is_jal || id_ex_is_jalr) {
        
        uint32_t alu_operand_a = id_ex_rs1_val;
        uint32_t alu_operand_b = id_ex_rs2_val;
        
        if (id_ex_is_alu_imm || id_ex_is_load || id_ex_is_jalr) {
            alu_operand_b = id_ex_imm;
        } else if (id_ex_is_lui) {
            alu_operand_a = 0;
            alu_operand_b = id_ex_imm;
        } else if (id_ex_is_auipc) {
            alu_operand_a = id_ex_pc;
            alu_operand_b = id_ex_imm;
        }
        
        alu.setOperandA(alu_operand_a);
        alu.setOperandB(alu_operand_b);
        alu.setAluOp(id_ex_alu_op);
        alu.setIsMul(0);
        alu.setMulOp(0);
        alu.setIsRem(0);
        alu.setRemUnsigned(0);
        alu.process();
        
        alu_result = alu.getAluResult();
        
        // Handle branch
        if (id_ex_is_branch) {
            branch_logic.setRs1Val(id_ex_rs1_val);
            branch_logic.setRs2Val(id_ex_rs2_val);
            branch_logic.setBranchOp(id_ex_branch_op);
            branch_logic.setBranchImm(id_ex_imm);
            branch_logic.setCurrentPc(id_ex_pc);
            branch_logic.process();
            
            branch_taken = branch_logic.getBranchTaken();
            branch_target = branch_logic.getBranchTarget();
            
            if (branch_taken) {
                next_pc = branch_target;
                flush_if_id = 1;
                flush_id_ex = 1;
                DPRINTF(RiscvCore, "Branch taken: PC set to 0x%x\n", next_pc);
            } else {
                next_pc = id_ex_pc + 4;
            }
            return next_pc;
        }
        
        // Handle JAL
        if (id_ex_is_jal) {
            if (id_ex_rd != 0) {
                reg_write = 1;
                rd = id_ex_rd;
                alu_result = id_ex_pc_plus4;
            }
            next_pc = id_ex_pc + id_ex_imm;
            flush_if_id = 1;
            flush_id_ex = 1;
            DPRINTF(RiscvCore, "JAL: PC set to 0x%x, ra=0x%x\n", next_pc, id_ex_pc_plus4);
            return next_pc;
        }
        
        // Handle JALR
        if (id_ex_is_jalr) {
            if (id_ex_rd != 0) {
                reg_write = 1;
                rd = id_ex_rd;
                alu_result = id_ex_pc_plus4;
            }
            next_pc = (id_ex_rs1_val + id_ex_imm) & ~1;
            flush_if_id = 1;
            flush_id_ex = 1;
            DPRINTF(RiscvCore, "JALR: PC set to 0x%x, ra=0x%x\n", next_pc, id_ex_pc_plus4);
            return next_pc;
        }
        
        // Handle LUI
        if (id_ex_is_lui) {
            if (id_ex_rd != 0) {
                reg_write = 1;
                rd = id_ex_rd;
            }
            return id_ex_pc + 4;
        }
        
        // Handle AUIPC
        if (id_ex_is_auipc) {
            if (id_ex_rd != 0) {
                reg_write = 1;
                rd = id_ex_rd;
            }
            return id_ex_pc + 4;
        }
        
        // Handle loads
        if (id_ex_is_load) {
            rib_data_addr_o_val = alu_result;
            rib_data_re_o_val = 1;
            rib_data_en_o_val = 1;
            rib_data_size_o_val = 2;
            rib_data_req_o_val = 1;
            
            if (rib_data_ready_i_reg && rib_data_re_o_val) {
                uint32_t load_data = rib_data_i_reg;
                reg_write = 1;
                rd = id_ex_rd;
                alu_result = load_data;
            } else if (id_ex_rd != 0) {
                reg_write = 1;
                rd = id_ex_rd;
            }
            return id_ex_pc + 4;
        }
        
        // Handle stores
        if (id_ex_is_store) {
            rib_data_addr_o_val = alu_result;
            rib_data_wdata_o_val = id_ex_rs2_val;
            rib_data_we_o_val = 1;
            rib_data_en_o_val = 1;
            rib_data_size_o_val = 2;
            rib_data_req_o_val = 1;
            return id_ex_pc + 4;
        }
        
        // Handle ALU reg/imm
        if (id_ex_is_alu_reg || id_ex_is_alu_imm) {
            if (id_ex_rd != 0) {
                reg_write = 1;
                rd = id_ex_rd;
            }
            return id_ex_pc + 4;
        }
    }
    
    // Handle NOP
    if (id_ex_is_nop) {
        return id_ex_pc + 4;
    }
    
    // Handle FENCE
    if (id_ex_is_fence) {
        return id_ex_pc + 4;
    }
    
    return id_ex_pc + 4;
}

uint32_t
RiscvCore::completeDivision()
{
    uint32_t result = 0;
    
    if (div_op == 0) {
        int32_t dividend = (int32_t)div_dividend;
        int32_t divisor = (int32_t)div_divisor;
        if (divisor == 0) {
            result = 0xFFFFFFFF;
        } else if (dividend == (int32_t)0x80000000 && divisor == -1) {
            result = 0x80000000;
        } else {
            result = (uint32_t)(dividend / divisor);
        }
    } else if (div_op == 1) {
        if (div_divisor == 0) {
            result = 0xFFFFFFFF;
        } else {
            result = div_dividend / div_divisor;
        }
    } else if (div_op == 2) {
        int32_t dividend = (int32_t)div_dividend;
        int32_t divisor = (int32_t)div_divisor;
        if (divisor == 0) {
            result = (uint32_t)dividend;
        } else if (dividend == (int32_t)0x80000000 && divisor == -1) {
            result = 0;
        } else {
            result = (uint32_t)(dividend % divisor);
        }
    } else {
        if (div_divisor == 0) {
            result = div_dividend;
        } else {
            result = div_dividend % div_divisor;
        }
    }
    
    return result;
}

uint32_t
RiscvCore::readReg(uint8_t addr)
{
    if (addr == 0) {
        return 0;
    }
    return regs[addr];
}

void
RiscvCore::writeReg(uint8_t addr, uint32_t data)
{
    if (addr != 0) {
        regs[addr] = data;
    }
}

uint32_t
RiscvCore::readCsr(uint32_t addr)
{
    if (addr == 0x300) return csr_mstatus;
    if (addr == 0x304) return csr_mie;
    if (addr == 0x344) return csr_mip;
    if (addr == 0x305) return csr_mtvec;
    if (addr == 0x340) return csr_mscratch;
    if (addr == 0x341) return csr_mepc;
    if (addr == 0x342) return csr_mcause;
    if (addr == 0xB00) return (uint32_t)csr_mcycle;
    if (addr == 0xB02) return (uint32_t)csr_minstret;
    return 0;
}

void
RiscvCore::writeCsr(uint32_t addr, uint32_t data)
{
    if (addr == 0x300) csr_mstatus = data;
    else if (addr == 0x304) csr_mie = data;
    else if (addr == 0x344) csr_mip = data;
    else if (addr == 0x305) csr_mtvec = data;
    else if (addr == 0x340) csr_mscratch = data;
    else if (addr == 0x341) csr_mepc = data;
    else if (addr == 0x342) csr_mcause = data;
    else if (addr == 0xB00) csr_mcycle = data;
    else if (addr == 0xB02) csr_minstret = data;
}

void
RiscvCore::updateDivision()
{
    div_count--;
    if (div_count <= 0) {
        div_busy = 0;
        div_done = 1;
        DPRINTF(RiscvCore, "Division complete, waiting for div_ready\n");
    }
}

void
RiscvCore::startDivision(uint32_t dividend, uint32_t divisor, uint8_t op)
{
    div_busy = 1;
    div_dividend = dividend;
    div_divisor = divisor;
    div_op = op;
    div_count = 32;
    DPRINTF(RiscvCore, "Division started: %u / %u, op=%d\n", dividend, divisor, op);
}

void
RiscvCore::setRst(uint8_t val)
{
    rst_i_reg = val;
}

void
RiscvCore::setRibInstI(uint32_t val)
{
    rib_inst_i_reg = val;
}

void
RiscvCore::setRibInstReadyI(uint8_t val)
{
    rib_inst_ready_i_reg = val;
}

void
RiscvCore::setRibDataI(uint32_t val)
{
    rib_data_i_reg = val;
}

void
RiscvCore::setRibDataReadyI(uint8_t val)
{
    rib_data_ready_i_reg = val;
}

void
RiscvCore::setRibDataAckI(uint8_t val)
{
    rib_data_ack_i_reg = val;
}

void
RiscvCore::setClintIntI(uint8_t val)
{
    clint_int_i_reg = val;
}

void
RiscvCore::setClintIntAssertI(uint8_t val)
{
    clint_int_assert_i_reg = val;
}

void
RiscvCore::setDivResultI(uint32_t val)
{
    div_result_i_reg = val;
}

void
RiscvCore::setDivReadyI(uint8_t val)
{
    div_ready_i_reg = val;
}

uint32_t
RiscvCore::getRibInstAddrO()
{
    return rib_inst_addr_o_val;
}

uint8_t
RiscvCore::getRibInstReqO()
{
    return rib_inst_req_o_val;
}

uint8_t
RiscvCore::getRibInstEnO()
{
    return rib_inst_en_o_val;
}

uint32_t
RiscvCore::getRibDataAddrO()
{
    return rib_data_addr_o_val;
}

uint32_t
RiscvCore::getRibDataWdataO()
{
    return rib_data_wdata_o_val;
}

uint8_t
RiscvCore::getRibDataWeO()
{
    return rib_data_we_o_val;
}

uint8_t
RiscvCore::getRibDataReO()
{
    return rib_data_re_o_val;
}

uint8_t
RiscvCore::getRibDataEnO()
{
    return rib_data_en_o_val;
}

uint8_t
RiscvCore::getRibDataSizeO()
{
    return rib_data_size_o_val;
}

uint8_t
RiscvCore::getRibDataReqO()
{
    return rib_data_req_o_val;
}

uint8_t
RiscvCore::getClintIntO()
{
    return clint_int_o_val;
}

uint8_t
RiscvCore::getClintIntAssertO()
{
    return clint_int_assert_o_val;
}

} // namespace gem5
