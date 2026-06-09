#ifndef __GENERATORS_ALU_TEST_GENERATOR_HH__
#define __GENERATORS_ALU_TEST_GENERATOR_HH__

#include "generators/riscv/alu/alu.hh"
#include "params/AluTestGenerator.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class AluTestGenerator : public SimObject {
private:
    Alu& dut;

    struct TestVector {
        uint32_t operand_a;
        uint32_t operand_b;
        uint8_t alu_op;
        uint8_t is_mul;
        uint8_t mul_op;
        uint8_t is_rem;
        uint8_t rem_unsigned;
        uint32_t expected_alu_result;
        uint8_t expected_branch_eq;
        uint8_t expected_branch_lt;
        uint8_t expected_branch_ltu;
        const char* description;
    };

    size_t passed;
    size_t failed;

    void runTests();

public:
    AluTestGenerator(const AluTestGeneratorParams &params);
    void startup() override;
};

} // namespace gem5

#endif // __GENERATORS_ALU_TEST_GENERATOR_HH__
