from m5.params import *
from m5.SimObject import SimObject

class HazardDetectionTestGenerator(SimObject):
    type = "HazardDetectionTestGenerator"
    cxx_header = "generators/riscv/hazard_detection_test/hazard_detection_test_generator.hh"
    cxx_class = "gem5::HazardDetectionTestGenerator"

    dut = Param.HazardDetection("Device under test")
