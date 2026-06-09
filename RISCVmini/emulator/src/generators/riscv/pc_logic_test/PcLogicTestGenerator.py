from m5.params import *
from m5.SimObject import SimObject


class PcLogicTestGenerator(SimObject):
    type = "PcLogicTestGenerator"
    cxx_header = "generators/riscv/pc_logic_test/pc_logic_test_generator.hh"
    cxx_class = "gem5::PcLogicTestGenerator"

    dut = Param.PcLogic("Device under test")
