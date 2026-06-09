#ifndef __GENERATORS_INSTRUCTION_DECODER_TEST_GENERATOR_HH__
#define __GENERATORS_INSTRUCTION_DECODER_TEST_GENERATOR_HH__

#include <cstdint>
#include <vector>

#include "generators/riscv/instruction_decoder/instruction_decoder.hh"
#include "params/InstructionDecoderTestGenerator.hh"
#include "sim/sim_object.hh"

namespace gem5 {

struct TestVector {
    uint32_t instruction;
    uint8_t opcode, funct3, funct7, rd, rs1, rs2;
    uint32_t imm_i, imm_s, imm_b, imm_u, imm_j;
    uint8_t is_r_type, is_i_type, is_s_type, is_b_type, is_u_type, is_j_type;
    uint8_t is_jalr, is_lui, is_auipc, is_load, is_store;
    uint8_t is_alu_reg, is_alu_imm, alu_op, mem_op, reg_write, mem_read, mem_write;
    uint8_t is_branch, branch_op, is_csr, csr_op, csr_use_imm;
    uint8_t is_mul, is_div, is_mret, is_ecall, is_ebreak, is_nop, is_fence;
};

class InstructionDecoderTestGenerator : public SimObject {
private:
    InstructionDecoder& dut;
    std::vector<TestVector> testVectors;
    size_t passed;
    size_t failed;

    void runTests();

public:
    InstructionDecoderTestGenerator(const InstructionDecoderTestGeneratorParams& params);
    void startup() override;
};

} // namespace gem5

#endif // __GENERATORS_INSTRUCTION_DECODER_TEST_GENERATOR_HH__
