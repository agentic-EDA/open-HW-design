#include "generators/riscv/hazard_detection_test/hazard_detection_test_generator.hh"

#include "base/trace.hh"
#include "debug/HazardDetectionTestGenerator.hh"
#include "sim/sim_exit.hh"

namespace gem5 {

HazardDetectionTestGenerator::HazardDetectionTestGenerator(
    const HazardDetectionTestGeneratorParams& params)
    : SimObject(params), dut(*params.dut), passed(0), failed(0) {
    testVectors = {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 3, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 2, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 3, 0, 0, 0, 0, 0, 0},
        {5, 0, 5, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 3, 0, 1, 1, 0, 1, 0},
        {0, 5, 5, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 3, 0, 1, 0, 1, 0, 1},
        {0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {5, 0, 5, 0, 1, 0, 1, 0, 1, 0, 0, 0, 0, 3, 0, 1, 1, 0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {5, 0, 5, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
        {0, 5, 5, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1},
        {5, 0, 0, 5, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1, 0},
        {0, 5, 0, 5, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 1},
        {5, 0, 5, 5, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0},
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {5, 0, 5, 0, 1, 0, 1, 0, 0, 0, 0, 0, 0, 3, 0, 1, 1, 0, 1, 0},
        {5, 0, 5, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1, 0},
        {0, 0, 0, 0, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {31, 31, 31, 31, 1, 1, 1, 0, 0, 0, 0, 0, 0, 3, 0, 1, 1, 1, 1, 1},
        {5, 6, 5, 6, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 1},
        {5, 0, 5, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 3, 0, 0, 1, 0, 1, 0},
        {5, 6, 5, 6, 1, 1, 1, 1, 1, 1, 1, 1, 1, 3, 0, 0, 1, 2, 1, 1},
        {10, 11, 10, 11, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 2, 1, 1},
        {5, 0, 5, 0, 1, 0, 1, 1, 0, 0, 0, 0, 0, 3, 0, 1, 1, 0, 1, 0},
    };
}

void
HazardDetectionTestGenerator::startup()
{
    DPRINTF(HazardDetectionTestGenerator, "Starting hazard detection tests\n");
    
    for (size_t i = 0; i < testVectors.size(); i++) {
        TestVector& vec = testVectors[i];
        
        dut.setIdRs1(vec.setIdRs1);
        dut.setIdRs2(vec.setIdRs2);
        dut.setExRd(vec.setExRd);
        dut.setWbRd(vec.setWbRd);
        dut.setExRegWrite(vec.setExRegWrite);
        dut.setWbRegWrite(vec.setWbRegWrite);
        dut.setExMemRead(vec.setExMemRead);
        dut.setIsBranch(vec.setIsBranch);
        dut.setIsJump(vec.setIsJump);
        dut.setBranchTaken(vec.setBranchTaken);
        dut.setDivBusy(vec.setDivBusy);
        dut.setMemBusy(vec.setMemBusy);
        dut.setInstNotReady(vec.setInstNotReady);
        dut.process();
        
        bool pass = true;
        if (dut.getHoldFlag() != vec.getHoldFlag) pass = false;
        if (dut.getFlushIfId() != vec.getFlushIfId) pass = false;
        if (dut.getFlushIdEx() != vec.getFlushIdEx) pass = false;
        if (dut.getForwardASel() != vec.getForwardASel) pass = false;
        if (dut.getForwardBSel() != vec.getForwardBSel) pass = false;
        if (dut.getCanForwardA() != vec.getCanForwardA) pass = false;
        if (dut.getCanForwardB() != vec.getCanForwardB) pass = false;
        
        if (pass) {
            passed++;
            DPRINTF(HazardDetectionTestGenerator, "Test TP%03d PASSED\n", (int)(i + 1));
        } else {
            failed++;
            DPRINTF(HazardDetectionTestGenerator, "Test TP%03d FAILED: hold=%d flush_if=%d flush_ex=%d fwd_a=%d fwd_b=%d can_a=%d can_b=%d\n",
                    (int)(i + 1), (int)dut.getHoldFlag(), (int)dut.getFlushIfId(), (int)dut.getFlushIdEx(),
                    (int)dut.getForwardASel(), (int)dut.getForwardBSel(),
                    (int)dut.getCanForwardA(), (int)dut.getCanForwardB());
        }
    }
    
    DPRINTF(HazardDetectionTestGenerator, "Tests complete: %d passed, %d failed, %d total\n",
            (int)passed, (int)failed, (int)testVectors.size());
    
    if (failed > 0) {
        panic("HazardDetectionTestGenerator: %d tests failed!", (int)failed);
    }
    
    exitSimLoop("HazardDetectionTestGenerator complete");
}

} // namespace gem5
