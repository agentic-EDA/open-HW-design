from m5.params import *
from m5.SimObject import SimObject


class BranchLogic(SimObject):
    type = "BranchLogic"
    cxx_header = "generators/riscv/branch_logic/branch_logic.hh"
    cxx_class = "gem5::BranchLogic"
