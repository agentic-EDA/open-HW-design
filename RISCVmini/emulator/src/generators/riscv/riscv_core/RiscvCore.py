from m5.params import *
from m5.SimObject import SimObject

from m5.objects import Alu
from m5.objects import BranchLogic
from m5.objects import ForwardingUnit
from m5.objects import HazardDetection
from m5.objects import InstructionDecoder
from m5.objects import PcLogic


class RiscvCore(SimObject):
    type = "RiscvCore"
    cxx_header = "generators/riscv/riscv_core/riscv_core.hh"
    cxx_class = "gem5::RiscvCore"

    alu = Param.Alu("ALU sub-module")
    branch_logic = Param.BranchLogic("Branch logic sub-module")
    forwarding_unit = Param.ForwardingUnit("Forwarding unit sub-module")
    hazard_detection = Param.HazardDetection("Hazard detection sub-module")
    instruction_decoder = Param.InstructionDecoder("Instruction decoder sub-module")
    pc_logic = Param.PcLogic("PC logic sub-module")

    clock_period = Param.Tick(1000, "Clock period in ticks")
    reset_addr = Param.UInt32(0x00000000, "Reset address")
    int_timer0_entry_addr = Param.UInt32(0x00000004, "Timer0 interrupt entry address")
