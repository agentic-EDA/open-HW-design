from m5.params import *
from m5.SimObject import SimObject


class CheckToVarTestGenerator(SimObject):
    type = "CheckToVarTestGenerator"
    cxx_header = "generators/ldpc/check_to_var_test/check_to_var_test_generator.hh"
    cxx_class = "gem5::CheckToVarTestGenerator"

    dut = Param.CheckToVar("Device under test")
