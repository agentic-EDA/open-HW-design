#ifndef __GENERATORS_PC_LOGIC_TEST_GENERATOR_HH__
#define __GENERATORS_PC_LOGIC_TEST_GENERATOR_HH__

#include <cstdint>
#include <vector>

#include "generators/riscv/pc_logic/pc_logic.hh"
#include "params/PcLogicTestGenerator.hh"
#include "sim/sim_object.hh"

namespace gem5 {

struct TestVector {
    uint8_t rst;
    uint32_t current_pc;
    uint32_t pc_plus4;
    uint32_t branch_target;
    uint32_t jump_target;
    uint32_t jalr_target;
    uint8_t is_jal;
    uint8_t is_jalr;
    uint8_t branch_taken;
    uint8_t is_exception;
    uint32_t exception_addr;
    uint8_t is_mret;
    uint32_t mret_addr;
    uint8_t hold_pc;
    uint8_t expected_pc_sel;
    uint32_t expected_next_pc;
};

class PcLogicTestGenerator : public SimObject {
private:
    PcLogic& dut;
    std::vector<TestVector> testVectors;
    size_t passed;
    size_t failed;

    void runTests();

public:
    PcLogicTestGenerator(const PcLogicTestGeneratorParams& params);
    void startup() override;
};

} // namespace gem5

#endif // __GENERATORS_PC_LOGIC_TEST_GENERATOR_HH__
