#ifndef __GENERATORS_INSTRUCTION_DECODER_INSTRUCTION_DECODER_HH__
#define __GENERATORS_INSTRUCTION_DECODER_INSTRUCTION_DECODER_HH__

#include <cstdint>

#include "params/InstructionDecoder.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class InstructionDecoderParams;

class InstructionDecoder : public SimObject {
private:
    uint32_t instruction_reg;

    uint8_t opcode_reg;
    uint8_t funct3_reg;
    uint8_t funct7_reg;
    uint8_t rd_reg;
    uint8_t rs1_reg;
    uint8_t rs2_reg;
    uint32_t imm_i_reg;
    uint32_t imm_s_reg;
    uint32_t imm_b_reg;
    uint32_t imm_u_reg;
    uint32_t imm_j_reg;
    uint8_t is_r_type_reg;
    uint8_t is_i_type_reg;
    uint8_t is_s_type_reg;
    uint8_t is_b_type_reg;
    uint8_t is_u_type_reg;
    uint8_t is_j_type_reg;
    uint8_t is_jalr_reg;
    uint8_t is_lui_reg;
    uint8_t is_auipc_reg;
    uint8_t is_load_reg;
    uint8_t is_store_reg;
    uint8_t is_alu_reg_reg;
    uint8_t is_alu_imm_reg;
    uint8_t alu_op_reg;
    uint8_t mem_op_reg;
    uint8_t reg_write_reg;
    uint8_t mem_read_reg;
    uint8_t mem_write_reg;
    uint8_t is_branch_reg;
    uint8_t branch_op_reg;
    uint8_t is_csr_reg;
    uint8_t csr_op_reg;
    uint8_t csr_use_imm_reg;
    uint8_t is_mul_reg;
    uint8_t is_div_reg;
    uint8_t is_mret_reg;
    uint8_t is_ecall_reg;
    uint8_t is_ebreak_reg;
    uint8_t is_nop_reg;
    uint8_t is_fence_reg;

    uint32_t signExtend(uint32_t value, int bits);
    uint32_t getITypeImm(uint32_t inst);
    uint32_t getSTypeImm(uint32_t inst);
    uint32_t getBTypeImm(uint32_t inst);
    uint32_t getUTypeImm(uint32_t inst);
    uint32_t getJTypeImm(uint32_t inst);

    uint8_t decodeAluOp(uint32_t inst, uint8_t opcode, uint8_t funct3, uint8_t funct7);
    uint8_t decodeMemOp(uint8_t funct3);
    uint8_t decodeBranchOp(uint8_t funct3);
    uint8_t decodeCsrOp(uint8_t funct3);

public:
    InstructionDecoder(const InstructionDecoderParams &params);

    void process();

    void setInstruction(uint32_t val);

    uint8_t getOpcode();
    uint8_t getFunct3();
    uint8_t getFunct7();
    uint8_t getRd();
    uint8_t getRs1();
    uint8_t getRs2();
    uint32_t getImmI();
    uint32_t getImmS();
    uint32_t getImmB();
    uint32_t getImmU();
    uint32_t getImmJ();
    uint8_t getIsRType();
    uint8_t getIsIType();
    uint8_t getIsSType();
    uint8_t getIsBType();
    uint8_t getIsUType();
    uint8_t getIsJType();
    uint8_t getIsJalr();
    uint8_t getIsLui();
    uint8_t getIsAuipc();
    uint8_t getIsLoad();
    uint8_t getIsStore();
    uint8_t getIsAluReg();
    uint8_t getIsAluImm();
    uint8_t getAluOp();
    uint8_t getMemOp();
    uint8_t getRegWrite();
    uint8_t getMemRead();
    uint8_t getMemWrite();
    uint8_t getIsBranch();
    uint8_t getBranchOp();
    uint8_t getIsCsr();
    uint8_t getCsrOp();
    uint8_t getCsrUseImm();
    uint8_t getIsMul();
    uint8_t getIsDiv();
    uint8_t getIsMret();
    uint8_t getIsEcall();
    uint8_t getIsEbreak();
    uint8_t getIsNop();
    uint8_t getIsFence();
};

} // namespace gem5

#endif // __GENERATORS_INSTRUCTION_DECODER_INSTRUCTION_DECODER_HH__
