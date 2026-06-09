#ifndef __GENERATORS_FORWARDING_UNIT_TEST_GENERATOR_HH__
#define __GENERATORS_FORWARDING_UNIT_TEST_GENERATOR_HH__

#include "generators/riscv/forwarding_unit/forwarding_unit.hh"
#include "params/ForwardingUnitTestGenerator.hh"
#include "sim/sim_object.hh"
#include <cstdint>
#include <vector>

namespace gem5 {

class ForwardingUnitTestGenerator : public SimObject {
private:
    ForwardingUnit& dut;

    struct TestVector {
        uint8_t id_rs1;
        uint8_t id_rs2;
        uint8_t ex_rd;
        uint8_t mem_rd;
        uint8_t ex_reg_write;
        uint8_t mem_reg_write;
        uint8_t ex_mem_read;
        uint8_t expected_forward_a_sel;
        uint8_t expected_forward_b_sel;
        const char* desc;
    };

    std::vector<TestVector> testVectors;
    size_t passed;
    size_t failed;

    void runTests();

public:
    ForwardingUnitTestGenerator(const ForwardingUnitTestGeneratorParams &params);
    void startup() override;
};

} // namespace gem5

#endif // __GENERATORS_FORWARDING_UNIT_TEST_GENERATOR_HH__
