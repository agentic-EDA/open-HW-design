from m5.params import *
from m5.SimObject import SimObject


class AluTestGenerator(SimObject):
    type = "AluTestGenerator"
    cxx_header = "generators/riscv/alu_test/alu_test_generator.hh"
    cxx_class = "gem5::AluTestGenerator"

    dut = Param.Alu("Device under test")
