import m5
from m5.objects import *

root = Root(full_system=False)

ch_evidence = ChEvidence()

var_to_check = [VarToCheck() for _ in range(25)]
check_to_var = [CheckToVar() for _ in range(25)]
belief = [Belief() for _ in range(10)]

root.decoder = Decoder(
    ch_evidence=ch_evidence,
    var_to_check=var_to_check,
    check_to_var=check_to_var,
    belief=belief,
)

root.tester = DecoderTestGenerator(dut=root.decoder, clock_period=100, max_iterations=5)

m5.instantiate()
exit_event = m5.simulate()
print(f"Exiting @ tick {m5.curTick()} because {exit_event.getCause()}")
