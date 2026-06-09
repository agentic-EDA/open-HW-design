from m5.params import *
from m5.SimObject import SimObject


class HazardDetection(SimObject):
    type = "HazardDetection"
    cxx_header = "generators/riscv/hazard_detection/hazard_detection.hh"
    cxx_class = "gem5::HazardDetection"
