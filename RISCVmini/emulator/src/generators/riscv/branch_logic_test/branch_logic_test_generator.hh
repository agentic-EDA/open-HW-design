#ifndef __GENERATORS_BRANCH_LOGIC_TEST_GENERATOR_HH__
#define __GENERATORS_BRANCH_LOGIC_TEST_GENERATOR_HH__

#include "generators/riscv/branch_logic/branch_logic.hh"
#include "params/BranchLogicTestGenerator.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class BranchLogicTestGenerator : public SimObject {
private:
    BranchLogic& dut;

    struct TestVector {
        const char* id;
        const char* desc;
        uint32_t rs1_val;
        uint32_t rs2_val;
        uint32_t branch_op;
        uint32_t branch_imm;
        uint32_t current_pc;
        uint32_t expected_branch_taken;
        uint32_t expected_branch_target;
    };

    static const TestVector test_vectors[];
    static const size_t num_test_vectors;

    size_t passed;
    size_t failed;

    void runTests();

public:
    BranchLogicTestGenerator(const BranchLogicTestGeneratorParams &params);
    void startup() override;
};

} // namespace gem5

#endif // __GENERATORS_BRANCH_LOGIC_TEST_GENERATOR_HH__
