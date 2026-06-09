from m5.params import *
from m5.SimObject import SimObject


class ChEvidence(SimObject):
    type = "ChEvidence"
    cxx_header = "generators/ldpc/ch_evidence/ch_evidence.hh"
    cxx_class = "gem5::ChEvidence"
