from m5.params import *
from m5.SimObject import SimObject


class PcLogic(SimObject):
    type = "PcLogic"
    cxx_header = "generators/riscv/pc_logic/pc_logic.hh"
    cxx_class = "gem5::PcLogic"
