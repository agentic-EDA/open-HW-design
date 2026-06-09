from m5.params import *
from m5.SimObject import SimObject


class InstructionDecoder(SimObject):
    type = "InstructionDecoder"
    cxx_header = "generators/riscv/instruction_decoder/instruction_decoder.hh"
    cxx_class = "gem5::InstructionDecoder"
