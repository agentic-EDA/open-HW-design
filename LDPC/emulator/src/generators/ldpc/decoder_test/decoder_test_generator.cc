#include "generators/ldpc/decoder_test/decoder_test_generator.hh"

#include "base/trace.hh"
#include "debug/DecoderTestGenerator.hh"
#include "sim/sim_exit.hh"

namespace gem5 {

const DecoderTestGenerator::TestVector
DecoderTestGenerator::testVectors[] = {
    {0x000, 13, 0x000, "TD001", "all_zeros"},
    {0x3FF, 13, 0x3FF, "TD002", "all_ones"},
    {0x001, 13, 0x000, "TD003", "single_bit_error"},
    {0x1A5, 13, 0x2EF, "TD004", "multi_bit_error"},
};

DecoderTestGenerator::DecoderTestGenerator(
    const DecoderTestGeneratorParams &params)
    : SimObject(params),
      event([this] { processEvent(); }, name()),
      clock_period(params.clock_period),
      dut(*params.dut),
      input_index(0),
      current_test(0),
      cycle_count(0),
      passed(0),
      failed(0),
      max_iterations(params.max_iterations)
{
}

void
DecoderTestGenerator::startup()
{
    DPRINTF(DecoderTestGenerator, "Starting decoder tests, total %d vectors\n", (int)numVectors);
    dut.setReceivedPacket(testVectors[0].received_packet);
    dut.setBitErrorLogProb(testVectors[0].bit_error_log_prob);
    schedule(event, clock_period);
}

void
DecoderTestGenerator::processEvent()
{
    if (cycle_count < max_iterations) {
        dut.processCycle();
        cycle_count++;
    }

    if (cycle_count >= max_iterations) {
        uint16_t corrected_seq = dut.getCorrectedSeq();
        uint16_t expected_seq = testVectors[current_test].expected_seq;
        uint16_t received = testVectors[current_test].received_packet;
        bool match = (corrected_seq == expected_seq);

        DPRINTF(DecoderTestGenerator, "Test %s  %s:  input=0x%03x  output=0x%03x  expect=0x%03x  %s\n",
                testVectors[current_test].seq_id,
                testVectors[current_test].desc,
                received, corrected_seq, expected_seq,
                match ? "PASSED" : "FAILED");
        if (match) passed++; else failed++;

        DPRINTF(DecoderTestGenerator, "  channel beliefs: %d %d %d %d %d %d %d %d %d %d\n",
                (int)dut.getChannelBelief(0), (int)dut.getChannelBelief(1),
                (int)dut.getChannelBelief(2), (int)dut.getChannelBelief(3),
                (int)dut.getChannelBelief(4), (int)dut.getChannelBelief(5),
                (int)dut.getChannelBelief(6), (int)dut.getChannelBelief(7),
                (int)dut.getChannelBelief(8), (int)dut.getChannelBelief(9));

        current_test++;
        cycle_count = 0;

        if (current_test >= numVectors) {
            DPRINTF(DecoderTestGenerator, "Tests complete: %d passed, %d failed, %d total\n",
                    (int)passed, (int)failed, (int)numVectors);
            if (failed > 0) {
                panic("DecoderTestGenerator: %d tests failed!", (int)failed);
            }
            exitSimLoop("DecoderTestGenerator complete");
            return;
        }

        dut.setReceivedPacket(testVectors[current_test].received_packet);
        dut.setBitErrorLogProb(testVectors[current_test].bit_error_log_prob);
    }

    if (!event.scheduled()) {
        schedule(event, curTick() + clock_period);
    }
}

} // namespace gem5
