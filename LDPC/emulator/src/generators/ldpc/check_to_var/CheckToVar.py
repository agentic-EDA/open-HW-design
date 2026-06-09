from m5.params import *
from m5.SimObject import SimObject


class CheckToVar(SimObject):
    type = "CheckToVar"
    cxx_header = "generators/ldpc/check_to_var/check_to_var.hh"
    cxx_class = "gem5::CheckToVar"
