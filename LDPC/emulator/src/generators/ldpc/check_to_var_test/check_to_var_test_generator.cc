#include "generators/ldpc/check_to_var_test/check_to_var_test_generator.hh"

#include "base/trace.hh"
#include "debug/CheckToVarTestGenerator.hh"
#include "sim/sim_exit.hh"

namespace gem5 {

const CheckToVarTestGenerator::TestVector
CheckToVarTestGenerator::testVectors[] = {
    {{5, 10, 3, 8}, 3, "all_positive_sign_even"},
    {{-5, -10, -3, -8}, 3, "all_negative_sign_even"},
    {{5, -10, 3, 8}, -3, "one_negative_sign_odd"},
    {{-5, -10, 3, 8}, 3, "two_negative_sign_even"},
    {{-5, -10, -3, 8}, -3, "three_negative_sign_odd"},
    {{0, 5, 10, 3}, 0, "contains_zero"},
};

CheckToVarTestGenerator::CheckToVarTestGenerator(
    const CheckToVarTestGeneratorParams &p)
    : SimObject(p), dut(*p.dut), passed(0), failed(0)
{
}

void
CheckToVarTestGenerator::runTests()
{
    for (size_t i = 0; i < numVectors; i++) {
        const auto& vec = testVectors[i];
        DPRINTF(CheckToVarTestGenerator, "Running test %d: %s\n", (int)i, vec.desc);

        uint64_t low = (static_cast<uint64_t>(static_cast<uint32_t>(vec.inputs[1])) << 32)
                     | static_cast<uint64_t>(static_cast<uint32_t>(vec.inputs[0]));
        uint64_t high = (static_cast<uint64_t>(static_cast<uint32_t>(vec.inputs[3])) << 32)
                      | static_cast<uint64_t>(static_cast<uint32_t>(vec.inputs[2]));

        dut.setNeighbourVarsLow(low);
        dut.setNeighbourVarsHigh(high);
        dut.process();

        int32_t actual = dut.getCheckToVarMessage();

        if (actual != vec.expected_output) {
            DPRINTF(CheckToVarTestGenerator,
                    "Test %d FAILED  %s: output=%d, expected %d "
                    "(inputs: %d,%d,%d,%d)\n",
                    (int)i, vec.desc, actual, vec.expected_output,
                    vec.inputs[0], vec.inputs[1], vec.inputs[2], vec.inputs[3]);
        }

        if (actual == vec.expected_output) {
            DPRINTF(CheckToVarTestGenerator,
                    "Test %d PASSED  %s\n",
                    (int)i, vec.desc);
            passed++;
        } else {
            failed++;
        }
    }

    DPRINTF(CheckToVarTestGenerator, "Tests complete: %d passed, %d failed, %d total\n",
            (int)passed, (int)failed, (int)numVectors);

    if (failed > 0) {
        panic("CheckToVarTestGenerator: %d tests failed!", (int)failed);
    }

    exitSimLoop("CheckToVarTestGenerator complete");
}

void
CheckToVarTestGenerator::startup()
{
    runTests();
}

} // namespace gem5
