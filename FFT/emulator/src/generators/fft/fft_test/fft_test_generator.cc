#include "generators/fft/fft_test/fft_test_generator.hh"

#include <cmath>
#include <cstdlib>

#include "base/trace.hh"
#include "debug/FftTestGenerator.hh"
#include "sim/sim_exit.hh"

namespace gem5 {

const int FftTestGenerator::NUM_POINTS;
const int FftTestGenerator::NUM_STAGES;
const int FftTestGenerator::NUM_TESTS;
const char* FftTestGenerator::test_names[NUM_TESTS] = {
    "complex single-tone (k=32, A=100)",
    "multi-tone with neg freq (k=10,100,-300)",
    "complex DC (1+j)*100",
    "complex impulse (1+j)*1024",
    "complex random noise"
};

FftTestGenerator::FftTestGenerator(const FftTestGeneratorParams &params) :
    SimObject(params),
    dut(*params.dut),
    clock_period(params.clock_period),
    input_index(0),
    output_count(0),
    phase(0),
    test_case(0),
    event([this]{processEvent();}, name()),
    passed(0),
    failed(0),
    total_passed(0),
    total_failed(0),
    expected_computed(false)
{
    input_real.resize(NUM_POINTS);
    input_imag.resize(NUM_POINTS);
    expected_real.resize(NUM_POINTS);
    expected_imag.resize(NUM_POINTS);
}

void
FftTestGenerator::startup()
{
    DPRINTF(FftTestGenerator, "Starting FFT test suite: %d test cases\n", NUM_TESTS);
    schedule(event, clock_period);
}

void
FftTestGenerator::generateInput(int n, int16_t& real, int16_t& imag)
{
    double pi = 3.141592653589793;

    switch (test_case) {
    case 0: {
        double a = 2.0 * pi * 32 * n / 1024;
        real = (int16_t)(100.0 * std::cos(a));
        imag = (int16_t)(100.0 * std::sin(a));
        break;
    }
    case 1: {
        int bins_pos[2] = {10, 100};
        int amps_pos[2] = {80, 50};
        double sum_r = 0, sum_i = 0;
        for (int b = 0; b < 2; b++) {
            double a = 2.0 * pi * bins_pos[b] * n / 1024;
            sum_r += amps_pos[b] * std::cos(a);
            sum_i += amps_pos[b] * std::sin(a);
        }
        double a_neg = -2.0 * pi * 300 * n / 1024;
        sum_r += 30.0 * std::cos(a_neg);
        sum_i += 30.0 * std::sin(a_neg);
        real = (int16_t)sum_r;
        imag = (int16_t)sum_i;
        break;
    }
    case 2:
        real = 100;
        imag = 100;
        break;
    case 3:
        real = (n == 0) ? 1024 : 0;
        imag = (n == 0) ? 1024 : 0;
        break;
    case 4:
        real = (int16_t)((7 * n + 13) % 200 - 100);
        imag = (int16_t)((11 * n + 37) % 200 - 100);
        break;
    }
}

void
FftTestGenerator::processEvent()
{
    if (phase == 0) {
        dut.setRst(1);
        dut.setDataValidI(0);
        dut.setClk(0);
        dut.setClk(1);
        phase = 1;
        schedule(event, curTick() + clock_period);
        return;
    }

    if (phase == 1) {
        dut.setRst(0);
        dut.setDataValidI(0);
        dut.setReadyI(1);
        dut.setClk(0);
        dut.setClk(1);
        phase = 2;
        schedule(event, curTick() + clock_period);
        return;
    }

    if (phase == 2) {
        if (input_index < 1024) {
            int16_t real_val, imag_val;
            generateInput((int)input_index, real_val, imag_val);
            input_real[input_index] = (uint16_t)real_val;
            input_imag[input_index] = (uint16_t)imag_val;
            dut.setDataValidI(1);
            dut.setDataRealI((uint16_t)real_val);
            dut.setDataImagI((uint16_t)imag_val);
            input_index++;
        } else {
            dut.setDataValidI(0);
        }

        dut.setClk(0);
        dut.setClk(1);

        if (input_index >= 1024) {
            if (!expected_computed) {
                DPRINTF(FftTestGenerator, "Test %d/%d: %s\n",
                        test_case + 1, NUM_TESTS, test_names[test_case]);
                computeReferenceFft();
                expected_computed = true;
            }
            phase = 3;
        }
        schedule(event, curTick() + clock_period);
        return;
    }

    if (phase == 3) {
        dut.setClk(0);
        dut.setClk(1);

        uint8_t valid_o = dut.getDataValidO();
        if (valid_o) {
            uint16_t real_o = dut.getDataRealO();
            uint16_t imag_o = dut.getDataImagO();
            uint16_t exp_real = expected_real[output_count];
            uint16_t exp_imag = expected_imag[output_count];

            int16_t r_o_s = (int16_t)real_o, i_o_s = (int16_t)imag_o;
            int16_t r_e_s = (int16_t)exp_real, i_e_s = (int16_t)exp_imag;
            int diff_r = std::abs((int)r_o_s - (int)r_e_s);
            int diff_i = std::abs((int)i_o_s - (int)i_e_s);
            int tol = 0;
            bool match = (diff_r <= tol) && (diff_i <= tol);
            if (match) {
                passed++;
            } else {
                failed++;
                DPRINTF(FftTestGenerator, "  sample %d FAILED: got=(%d,%d) exp=(%d,%d) diff=(%d,%d)\n",
                        (int)output_count, (int16_t)real_o, (int16_t)imag_o,
                        (int16_t)exp_real, (int16_t)exp_imag, diff_r, diff_i);
            }
            output_count++;

            if (output_count >= 1024) {
                DPRINTF(FftTestGenerator, "  -> %s: %d passed, %d failed\n",
                        test_names[test_case], (int)passed, (int)failed);
                total_passed += passed;
                total_failed += failed;

                test_case++;
                if (test_case >= NUM_TESTS) {
                    DPRINTF(FftTestGenerator, "FFT test suite complete: %d passed, %d failed, %d total\n",
                            (int)total_passed, (int)total_failed, (int)(total_passed + total_failed));
                    if (total_failed > 0) {
                        panic("FftTestGenerator: %d total failures!", (int)total_failed);
                    }
                    exitSimLoop("FftTestGenerator complete");
                    return;
                }

                input_index = 0;
                output_count = 0;
                passed = 0;
                failed = 0;
                expected_computed = false;
                phase = 0;
                schedule(event, curTick() + clock_period);
                return;
            }
        }
        schedule(event, curTick() + clock_period);
    }
}

int16_t
FftTestGenerator::saturateRef(int32_t val)
{
    if (val > INT16_MAX) return INT16_MAX;
    if (val < INT16_MIN) return INT16_MIN;
    return (int16_t)val;
}

void
FftTestGenerator::computeReferenceFft()
{
    double pi = 3.14159265358979323846;
    int n = NUM_POINTS;

    for (int k = 0; k < n; k++) {
        double sum_r = 0, sum_i = 0;
        for (int t = 0; t < n; t++) {
            double angle = -2.0 * pi * k * t / n;
            double w_r = std::cos(angle);
            double w_i = std::sin(angle);
            double x_r = (int16_t)input_real[t];
            double x_i = (int16_t)input_imag[t];
            sum_r += x_r * w_r - x_i * w_i;
            sum_i += x_r * w_i + x_i * w_r;
        }
        int16_t r_sat = saturateRef((int32_t)std::round(sum_r / n));
        int16_t i_sat = saturateRef((int32_t)std::round(sum_i / n));
        expected_real[k] = (uint16_t)r_sat;
        expected_imag[k] = (uint16_t)i_sat;
    }
}

} // namespace gem5
 
