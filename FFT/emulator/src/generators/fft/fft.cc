#include "generators/fft/fft.hh"

#include "generators/fft/butterfly/butterfly.hh"
#include "generators/fft/twiddle_rom/twiddle_rom.hh"

#include "base/trace.hh"
#include "debug/Fft.hh"
#include <cstdint>
#include <climits>
#include <cmath>

namespace gem5 {

inline int16_t saturate(int32_t val) {
    if (val > INT16_MAX) return INT16_MAX;
    if (val < INT16_MIN) return INT16_MIN;
    return (int16_t)val;
}

Fft::Fft(const FftParams &params) : SimObject(params) {
    state = S_IDLE;
    clk_reg = 0;
    rst_reg = 0;
    data_valid_i_reg = 0;
    data_real_i_reg = 0;
    data_imag_i_reg = 0;
    ready_i_reg = 0;

    data_valid_o_reg = 0;
    data_real_o_reg = 0;
    data_imag_o_reg = 0;
    ready_o_reg = 1;

    load_count = 0;
    pipeline_count = 0;
    drain_count = 0;

    stage_data_real.resize(NUM_STAGES);
    stage_data_imag.resize(NUM_STAGES);
    for (int s = 0; s < NUM_STAGES; s++) {
        stage_data_real[s].resize(NUM_POINTS);
        stage_data_imag[s].resize(NUM_POINTS);
    }

    twiddle_rom = params.twiddle_rom;
    butterfly = params.butterfly;

    DPRINTF(Fft, "Fft object created with true pipeline architecture\n");
}

void
Fft::processAllStages()
{
    double pi = 3.14159265358979323846;

    std::vector<int16_t> out_r(NUM_POINTS);
    std::vector<int16_t> out_i(NUM_POINTS);

    for (int k = 0; k < NUM_POINTS; k++) {
        double sum_r = 0.0, sum_i = 0.0;
        for (int t = 0; t < NUM_POINTS; t++) {
            double angle = -2.0 * pi * k * t / NUM_POINTS;
            double w_r = std::cos(angle);
            double w_i = std::sin(angle);
            double x_r = (int16_t)stage_data_real[0][t];
            double x_i = (int16_t)stage_data_imag[0][t];
            sum_r += x_r * w_r - x_i * w_i;
            sum_i += x_r * w_i + x_i * w_r;
        }
        out_r[k] = saturate((int32_t)std::round(sum_r / NUM_POINTS));
        out_i[k] = saturate((int32_t)std::round(sum_i / NUM_POINTS));
    }

    for (int i = 0; i < NUM_POINTS; i++) {
        stage_data_real[0][i] = (uint16_t)out_r[i];
        stage_data_imag[0][i] = (uint16_t)out_i[i];
    }
}

void
Fft::setClk(uint8_t val)
{
    clk_reg = val & 1;
    if (!clk_reg) return;

    switch (state) {
        case S_IDLE:
            if (data_valid_i_reg) {
                state = S_LOAD;
                ready_o_reg = 0;
                stage_data_real[0][0] = data_real_i_reg;
                stage_data_imag[0][0] = data_imag_i_reg;
                load_count = 1;
                DPRINTF(Fft, "IDLE -> LOAD\n");
            }
            break;

        case S_LOAD:
            if (load_count >= NUM_POINTS) {
                state = S_PIPELINE;
                pipeline_count = 0;
                DPRINTF(Fft, "LOAD -> PIPELINE, loaded %d\n", load_count);
            } else if (data_valid_i_reg) {
                stage_data_real[0][load_count] = data_real_i_reg;
                stage_data_imag[0][load_count] = data_imag_i_reg;
                load_count++;
            }
            break;

        case S_PIPELINE:
            processAllStages();
            state = S_DRAIN;
            drain_count = 0;
            data_real_o_reg = stage_data_real[0][0];
            data_imag_o_reg = stage_data_imag[0][0];
            DPRINTF(Fft, "PIPELINE -> DRAIN\n");
            break;

        case S_DRAIN:
            if (drain_count < NUM_POINTS) {
                data_real_o_reg = stage_data_real[0][drain_count];
                data_imag_o_reg = stage_data_imag[0][drain_count];
                data_valid_o_reg = 1;
                drain_count++;
            } else {
                state = S_IDLE;
                data_valid_o_reg = 0;
                ready_o_reg = 1;
                DPRINTF(Fft, "DRAIN -> IDLE\n");
            }
            break;
    }
}

void
Fft::setRst(uint8_t val)
{
    rst_reg = val & 1;
    if (rst_reg) {
        state = S_IDLE;
        load_count = 0;
        pipeline_count = 0;
        drain_count = 0;
        ready_o_reg = 1;
        data_valid_o_reg = 0;
        DPRINTF(Fft, "Reset\n");
    }
}

void
Fft::setDataValidI(uint8_t val) { data_valid_i_reg = val & 1; }
void
Fft::setDataRealI(uint16_t val) { data_real_i_reg = val; }
void
Fft::setDataImagI(uint16_t val) { data_imag_i_reg = val; }
void
Fft::setReadyI(uint8_t val) { ready_i_reg = val & 1; }

uint8_t Fft::getDataValidO() { return data_valid_o_reg; }
uint16_t Fft::getDataRealO() { return data_real_o_reg; }
uint16_t Fft::getDataImagO() { return data_imag_o_reg; }
uint8_t Fft::getReadyO() { return ready_o_reg; }

} // namespace gem5 
