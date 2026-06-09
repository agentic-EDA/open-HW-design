from m5.params import *
from m5.SimObject import SimObject


class BranchLogicTestGenerator(SimObject):
    type = "BranchLogicTestGenerator"
    cxx_header = "generators/riscv/branch_logic_test/branch_logic_test_generator.hh"
    cxx_class = "gem5::BranchLogicTestGenerator"

    dut = Param.BranchLogic("Device under test")
