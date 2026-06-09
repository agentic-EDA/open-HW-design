from m5.params import *
from m5.SimObject import SimObject


class BeliefTestGenerator(SimObject):
    type = "BeliefTestGenerator"
    cxx_header = "generators/ldpc/belief_test/belief_test_generator.hh"
    cxx_class = "gem5::BeliefTestGenerator"

    dut = Param.Belief("Device under test")
