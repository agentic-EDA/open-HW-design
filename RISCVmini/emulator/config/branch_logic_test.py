import m5
from m5.objects import *

root = Root(full_system=False)
root.dut = BranchLogic()
root.tester = BranchLogicTestGenerator(dut=root.dut)

m5.instantiate()
exit_event = m5.simulate()
print(f"Exiting @ tick {m5.curTick()} because {exit_event.getCause()}")
