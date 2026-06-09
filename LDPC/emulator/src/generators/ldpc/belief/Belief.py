from m5.params import *
from m5.SimObject import SimObject


class Belief(SimObject):
    type = "Belief"
    cxx_header = "generators/ldpc/belief/belief.hh"
    cxx_class = "gem5::Belief"
