from m5.params import *
from m5.SimObject import SimObject


class FftTestGenerator(SimObject):
    type = "FftTestGenerator"
    cxx_header = "generators/fft/fft_test/fft_test_generator.hh"
    cxx_class = "gem5::FftTestGenerator"

    clock_period = Param.Tick(100, "Clock period")
    dut = Param.Fft("Device under test")
