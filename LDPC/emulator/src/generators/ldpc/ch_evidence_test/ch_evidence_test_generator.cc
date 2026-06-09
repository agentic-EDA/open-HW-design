#include "generators/ldpc/ch_evidence_test/ch_evidence_test_generator.hh"

#include "base/trace.hh"
#include "debug/ChEvidenceTestGenerator.hh"
#include "sim/sim_exit.hh"

namespace gem5 {

const ChEvidenceTestGenerator::TestVector
ChEvidenceTestGenerator::testVectors[] = {
    {0x000, 13, {13, 13, 13, 13, 13, 13, 13, 13, 13, 13}, "all_zeros"},
    {0x3FF, 13, {-13, -13, -13, -13, -13, -13, -13, -13, -13, -13}, "all_ones"},
    {0x1A5, 13, {-13, 13, -13, 13, 13, -13, 13, -13, -13, 13}, "mixed_pattern"},
    {0x3FF, 0, {0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, "zero_logprob"},
    {0x001, 13, {-13, 13, 13, 13, 13, 13, 13, 13, 13, 13}, "single_bit"},
};

ChEvidenceTestGenerator::ChEvidenceTestGenerator(
    const ChEvidenceTestGeneratorParams &p)
    : SimObject(p), dut(*p.dut), passed(0), failed(0)
{
}

void
ChEvidenceTestGenerator::runTests()
{
    for (size_t i = 0; i < numVectors; i++) {
        const auto& vec = testVectors[i];
        DPRINTF(ChEvidenceTestGenerator, "Running test %d: %s\n", (int)i, vec.desc);

        dut.setReceivedPacket(vec.received_packet);
        dut.setBitErrorLogProb(vec.bit_error_log_prob);
        dut.process();

        bool pass = true;
        for (int j = 0; j < 10; j++) {
            int32_t actual = dut.getChannelEvidence(j);
            if (actual != vec.expected_evidence[j]) {
                pass = false;
                DPRINTF(ChEvidenceTestGenerator,
                        "  mismatch ch[%d]: got %d, expected %d\n",
                        j, actual, vec.expected_evidence[j]);
            }
        }

        if (pass) {
            passed++;
            DPRINTF(ChEvidenceTestGenerator, "Test %d PASSED  %s\n",
                    (int)i, vec.desc);
        } else {
            failed++;
            DPRINTF(ChEvidenceTestGenerator, "Test %d FAILED  %s\n",
                    (int)i, vec.desc);
        }
    }

    DPRINTF(ChEvidenceTestGenerator, "Tests complete: %d passed, %d failed, %d total\n",
            (int)passed, (int)failed, (int)numVectors);

    if (failed > 0) {
        panic("ChEvidenceTestGenerator: %d tests failed!", (int)failed);
    }

    exitSimLoop("ChEvidenceTestGenerator complete");
}

void
ChEvidenceTestGenerator::startup()
{
    runTests();
}

} // namespace gem5
