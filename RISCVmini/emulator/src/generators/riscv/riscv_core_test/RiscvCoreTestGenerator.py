from m5.params import *
from m5.SimObject import SimObject


class RiscvCoreTestGenerator(SimObject):
    type = "RiscvCoreTestGenerator"
    cxx_header = "generators/riscv/riscv_core_test/riscv_core_test_generator.hh"
    cxx_class = "gem5::RiscvCoreTestGenerator"

    dut = Param.RiscvCore("Device under test")
