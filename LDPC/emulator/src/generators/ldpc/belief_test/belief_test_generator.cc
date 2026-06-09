#include "generators/ldpc/belief_test/belief_test_generator.hh"

#include "base/trace.hh"
#include "debug/BeliefTestGenerator.hh"
#include "sim/sim_exit.hh"

namespace gem5 {

const BeliefTestGenerator::TestVector
BeliefTestGenerator::testVectors[] = {
    {{5, 10, 3, 8}, 26, 0, "all_positive"},
    {{-5, -10, -3, -8}, -26, 1, "all_negative"},
    {{5, -3, 7, -2}, 7, 0, "mixed_positive_result"},
    {{-5, 3, -7, 2}, -7, 1, "mixed_negative_result"},
    {{5, -5, 3, -3}, 0, 1, "belief_zero"},
    {{0, 0, 0, 0}, 0, 1, "all_zeros"},
};

BeliefTestGenerator::BeliefTestGenerator(
    const BeliefTestGeneratorParams &p)
    : SimObject(p), dut(*p.dut), passed(0), failed(0)
{
}

void
BeliefTestGenerator::runTests()
{
    for (size_t i = 0; i < numVectors; i++) {
        const auto& vec = testVectors[i];
        DPRINTF(BeliefTestGenerator, "Running test %d: %s\n", (int)i, vec.desc);

        uint64_t low = (static_cast<uint64_t>(static_cast<uint32_t>(vec.inputs[1])) << 32)
                     | static_cast<uint64_t>(static_cast<uint32_t>(vec.inputs[0]));
        uint64_t high = (static_cast<uint64_t>(static_cast<uint32_t>(vec.inputs[3])) << 32)
                      | static_cast<uint64_t>(static_cast<uint32_t>(vec.inputs[2]));

        dut.setNeighbourChecksLow(low);
        dut.setNeighbourChecksHigh(high);
        dut.process();

        int32_t actual_belief = dut.getBelief();
        uint32_t actual_corrected_bit = dut.getCorrectedBit();

        bool pass = true;

        bool belief_ok = (actual_belief == vec.expected_belief);
        if (!belief_ok) {
            DPRINTF(BeliefTestGenerator,
                    "Test %d FAILED  %s: belief=%d, expected %d\n",
                    (int)i, vec.desc, actual_belief, vec.expected_belief);
        }

        bool corrected_ok = (actual_corrected_bit == vec.expected_corrected_bit);
        if (!corrected_ok) {
            DPRINTF(BeliefTestGenerator,
                    "Test %d FAILED  %s: corrected_bit=%u, expected %u\n",
                    (int)i, vec.desc, actual_corrected_bit, vec.expected_corrected_bit);
        }

        if (belief_ok && corrected_ok) {
            DPRINTF(BeliefTestGenerator, "Test %d PASSED  %s\n", (int)i, vec.desc);
            passed++;
        } else {
            failed++;
        }
    }

    DPRINTF(BeliefTestGenerator, "Tests complete: %d passed, %d failed, %d total\n",
            (int)passed, (int)failed, (int)numVectors);

    if (failed > 0) {
        panic("BeliefTestGenerator: %d tests failed!", (int)failed);
    }

    exitSimLoop("BeliefTestGenerator complete");
}

void
BeliefTestGenerator::startup()
{
    runTests();
}

} // namespace gem5
