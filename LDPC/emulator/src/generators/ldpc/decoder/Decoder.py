from m5.params import *
from m5.SimObject import SimObject


class Decoder(SimObject):
    type = "Decoder"
    cxx_header = "generators/ldpc/decoder/decoder.hh"
    cxx_class = "gem5::Decoder"

    ch_evidence = Param.ChEvidence("Channel evidence module")
    var_to_check = VectorParam.VarToCheck("Variable to check modules (25)")
    check_to_var = VectorParam.CheckToVar("Check to variable modules (25)")
    belief = VectorParam.Belief("Belief modules (10)")
