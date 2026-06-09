import m5
from m5.objects import *

root = Root(full_system=False)

# Instantiate sub-modules
root.dut = RiscvCore(
    alu=Alu(),
    branch_logic=BranchLogic(),
    forwarding_unit=ForwardingUnit(),
    hazard_detection=HazardDetection(),
    instruction_decoder=InstructionDecoder(),
    pc_logic=PcLogic(),
)
root.tester = RiscvCoreTestGenerator(dut=root.dut)

m5.instantiate()
exit_event = m5.simulate()
print(f"Exiting @ tick {m5.curTick()} because {exit_event.getCause()}")
