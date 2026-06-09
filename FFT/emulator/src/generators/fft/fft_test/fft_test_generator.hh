#ifndef __GENERATORS_FFT_FFT_TEST_FFT_TEST_GENERATOR_HH__
#define __GENERATORS_FFT_FFT_TEST_FFT_TEST_GENERATOR_HH__

#include <cstdint>
#include <vector>

#include "generators/fft/fft.hh"
#include "params/FftTestGenerator.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class FftTestGenerator : public SimObject {
private:
    Fft& dut;
    const Tick clock_period;
    size_t input_index;
    size_t output_count;
    int phase;
    int test_case;
    EventFunctionWrapper event;
    size_t passed;
    size_t failed;
    size_t total_passed;
    size_t total_failed;

    void processEvent();
    void computeReferenceFft();
    int16_t saturateRef(int32_t val);
    void generateInput(int n, int16_t& real, int16_t& imag);

    std::vector<uint16_t> input_real;
    std::vector<uint16_t> input_imag;
    std::vector<uint16_t> expected_real;
    std::vector<uint16_t> expected_imag;
    bool expected_computed;

    static const int NUM_POINTS = 1024;
    static const int NUM_STAGES = 5;
    static const int NUM_TESTS = 5;

    static const char* test_names[NUM_TESTS];

public:
    FftTestGenerator(const FftTestGeneratorParams &params);
    void startup() override;
};

} // namespace gem5

#endif // __GENERATORS_FFT_FFT_TEST_FFT_TEST_GENERATOR_HH__