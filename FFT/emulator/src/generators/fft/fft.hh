#ifndef __GENERATORS_FFT_FFT_HH__
#define __GENERATORS_FFT_FFT_HH__

#include <cstdint>
#include <vector>

#include "params/Fft.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class FftParams;
class Butterfly;
class TwiddleRom;

class Fft : public SimObject {
private:
    static const int NUM_POINTS = 1024;
    static const int NUM_STAGES = 5;
    static const int BUTTERFLY_PER_STAGE = 256;

    enum State {
        S_IDLE = 0,
        S_LOAD = 1,
        S_PIPELINE = 2,
        S_DRAIN = 3
    };

    State state;
    uint8_t clk_reg;
    uint8_t rst_reg;
    uint8_t data_valid_i_reg;
    uint16_t data_real_i_reg;
    uint16_t data_imag_i_reg;
    uint8_t ready_i_reg;

    uint8_t data_valid_o_reg;
    uint16_t data_real_o_reg;
    uint16_t data_imag_o_reg;
    uint8_t ready_o_reg;

    int load_count;
    int pipeline_count;
    int drain_count;

    std::vector<std::vector<uint16_t>> stage_data_real;
    std::vector<std::vector<uint16_t>> stage_data_imag;

    TwiddleRom* twiddle_rom;
    Butterfly* butterfly;

    void processAllStages();

public:
    Fft(const FftParams &params);

    void setClk(uint8_t val);
    void setRst(uint8_t val);
    void setDataValidI(uint8_t val);
    void setDataRealI(uint16_t val);
    void setDataImagI(uint16_t val);
    void setReadyI(uint8_t val);

    uint8_t getDataValidO();
    uint16_t getDataRealO();
    uint16_t getDataImagO();
    uint8_t getReadyO();
};

} // namespace gem5

#endif // __GENERATORS_FFT_FFT_HH__
