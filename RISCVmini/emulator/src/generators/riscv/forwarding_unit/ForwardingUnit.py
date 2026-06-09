from m5.params import *
from m5.SimObject import SimObject


class ForwardingUnit(SimObject):
    type = "ForwardingUnit"
    cxx_header = "generators/riscv/forwarding_unit/forwarding_unit.hh"
    cxx_class = "gem5::ForwardingUnit"
