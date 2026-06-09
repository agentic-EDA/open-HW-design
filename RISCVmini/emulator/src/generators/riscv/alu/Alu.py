from m5.params import *
from m5.SimObject import SimObject


class Alu(SimObject):
    type = "Alu"
    cxx_header = "generators/riscv/alu/alu.hh"
    cxx_class = "gem5::Alu"
