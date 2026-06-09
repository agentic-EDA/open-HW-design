from m5.params import *
from m5.SimObject import SimObject


class ForwardingUnitTestGenerator(SimObject):
    type = "ForwardingUnitTestGenerator"
    cxx_header = "generators/riscv/forwarding_unit_test/forwarding_unit_test_generator.hh"
    cxx_class = "gem5::ForwardingUnitTestGenerator"

    dut = Param.ForwardingUnit("Device under test")
