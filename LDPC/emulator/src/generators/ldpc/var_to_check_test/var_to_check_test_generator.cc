#include "generators/ldpc/var_to_check_test/var_to_check_test_generator.hh"

#include "base/trace.hh"
#include "debug/VarToCheckTestGenerator.hh"
#include "sim/sim_exit.hh"

namespace gem5 {

const VarToCheckTestGenerator::TestVector
VarToCheckTestGenerator::testVectors[] = {
    {{0, 0, 0, 0}, 0, "all_zeros"},
    {{10, 10, 10, 10}, 40, "all_positive"},
    {{-10, -10, -10, -10}, -40, "all_negative"},
    {{5, -3, 7, -2}, 7, "mixed_values"},
    {{1000000, 1000000, 1000000, 1000000}, 4000000, "large_positive"},
    {{2147483647, -2147483647, 0, 0}, 0, "max_min_cancel"},
};

VarToCheckTestGenerator::VarToCheckTestGenerator(
    const VarToCheckTestGeneratorParams &p)
    : SimObject(p), dut(*p.dut), passed(0), failed(0)
{
}

void
VarToCheckTestGenerator::runTests()
{
    for (size_t i = 0; i < numVectors; i++) {
        const auto& vec = testVectors[i];
        DPRINTF(VarToCheckTestGenerator, "Running test %d: %s\n", (int)i, vec.desc);

        uint64_t low = (static_cast<uint64_t>(static_cast<uint32_t>(vec.inputs[1])) << 32)
                     | static_cast<uint64_t>(static_cast<uint32_t>(vec.inputs[0]));
        uint64_t high = (static_cast<uint64_t>(static_cast<uint32_t>(vec.inputs[3])) << 32)
                      | static_cast<uint64_t>(static_cast<uint32_t>(vec.inputs[2]));

        dut.setNeighbourChecksLow(low);
        dut.setNeighbourChecksHigh(high);
        dut.process();

        int32_t actual = dut.getVarToCheckMessage();

        if (actual != vec.expected_sum) {
            DPRINTF(VarToCheckTestGenerator,
                    "Test %d FAILED  %s: sum=%d, expected %d "
                    "(inputs: %d,%d,%d,%d)\n",
                    (int)i, vec.desc, actual, vec.expected_sum,
                    vec.inputs[0], vec.inputs[1], vec.inputs[2], vec.inputs[3]);
        }

        if (actual == vec.expected_sum) {
            DPRINTF(VarToCheckTestGenerator,
                    "Test %d PASSED  %s\n",
                    (int)i, vec.desc);
            passed++;
        } else {
            failed++;
        }
    }

    DPRINTF(VarToCheckTestGenerator, "Tests complete: %d passed, %d failed, %d total\n",
            (int)passed, (int)failed, (int)numVectors);

    if (failed > 0) {
        panic("VarToCheckTestGenerator: %d tests failed!", (int)failed);
    }

    exitSimLoop("VarToCheckTestGenerator complete");
}

void
VarToCheckTestGenerator::startup()
{
    runTests();
}

} // namespace gem5
