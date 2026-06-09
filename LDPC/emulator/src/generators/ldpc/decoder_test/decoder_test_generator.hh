#ifndef __GENERATORS_LDPC_DECODER_TEST_GENERATOR_HH__
#define __GENERATORS_LDPC_DECODER_TEST_GENERATOR_HH__

#include "generators/ldpc/decoder/decoder.hh"
#include "params/DecoderTestGenerator.hh"
#include "sim/eventq.hh"
#include "sim/sim_object.hh"

#include <cstddef>
#include <cstdint>

namespace gem5 {

class DecoderTestGenerator : public SimObject {
  private:
    void processEvent();
    EventFunctionWrapper event;
    const Tick clock_period;
    Decoder &dut;
    size_t input_index;
    size_t current_test;
    int cycle_count;
    size_t passed;
    size_t failed;
    const int max_iterations;

    struct TestVector {
        uint16_t received_packet;
        int32_t bit_error_log_prob;
        uint16_t expected_seq;
        const char* seq_id;
        const char* desc;
    };

    static const TestVector testVectors[];
    static constexpr size_t numVectors = 4;

  public:
    DecoderTestGenerator(const DecoderTestGeneratorParams &params);
    void startup() override;
};

} // namespace gem5

#endif // __GENERATORS_LDPC_DECODER_TEST_GENERATOR_HH__
