from m5.params import *
from m5.SimObject import SimObject


class InstructionDecoderTestGenerator(SimObject):
    type = "InstructionDecoderTestGenerator"
    cxx_header = "generators/riscv/instruction_decoder_test/instruction_decoder_test_generator.hh"
    cxx_class = "gem5::InstructionDecoderTestGenerator"

    dut = Param.InstructionDecoder("Device under test")
