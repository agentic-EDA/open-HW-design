from m5.params import *
from m5.SimObject import SimObject


class DecoderTestGenerator(SimObject):
    type = "DecoderTestGenerator"
    cxx_header = "generators/ldpc/decoder_test/decoder_test_generator.hh"
    cxx_class = "gem5::DecoderTestGenerator"

    clock_period = Param.Tick(100, "Clock period")
    dut = Param.Decoder("Device under test")
    max_iterations = Param.Int(5, "Maximum iterations")
