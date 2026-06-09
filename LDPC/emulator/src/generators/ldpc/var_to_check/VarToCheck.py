from m5.params import *
from m5.SimObject import SimObject


class VarToCheck(SimObject):
    type = "VarToCheck"
    cxx_header = "generators/ldpc/var_to_check/var_to_check.hh"
    cxx_class = "gem5::VarToCheck"
