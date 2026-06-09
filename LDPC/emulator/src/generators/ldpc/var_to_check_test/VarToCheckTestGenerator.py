from m5.params import *
from m5.SimObject import SimObject


class VarToCheckTestGenerator(SimObject):
    type = "VarToCheckTestGenerator"
    cxx_header = "generators/ldpc/var_to_check_test/var_to_check_test_generator.hh"
    cxx_class = "gem5::VarToCheckTestGenerator"

    dut = Param.VarToCheck("Device under test")
