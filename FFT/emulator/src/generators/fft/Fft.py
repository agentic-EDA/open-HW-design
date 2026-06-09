from m5.params import *
from m5.SimObject import SimObject


class Fft(SimObject):
    type = "Fft"
    cxx_header = "generators/fft/fft.hh"
    cxx_class = "gem5::Fft"

    num_points = Param.Int(1024, "Number of FFT points")
    num_stages = Param.Int(5, "Number of FFT stages")

    butterfly = Param.Butterfly("Butterfly unit for radix-4 computation")
    twiddle_rom = Param.TwiddleRom("Twiddle factor ROM")
