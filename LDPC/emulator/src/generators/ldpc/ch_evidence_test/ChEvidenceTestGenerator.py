from m5.params import *
from m5.SimObject import SimObject


class ChEvidenceTestGenerator(SimObject):
    type = "ChEvidenceTestGenerator"
    cxx_header = "generators/ldpc/ch_evidence_test/ch_evidence_test_generator.hh"
    cxx_class = "gem5::ChEvidenceTestGenerator"

    dut = Param.ChEvidence("Device under test")
