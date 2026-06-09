#ifndef __GENERATORS_HAZARD_DETECTION_TEST_GENERATOR_HH__
#define __GENERATORS_HAZARD_DETECTION_TEST_GENERATOR_HH__

#include <cstdint>
#include <vector>

#include "generators/riscv/hazard_detection/hazard_detection.hh"
#include "params/HazardDetectionTestGenerator.hh"
#include "sim/sim_object.hh"

namespace gem5 {

struct TestVector {
    // Inputs
    uint8_t setIdRs1;
    uint8_t setIdRs2;
    uint8_t setExRd;
    uint8_t setWbRd;
    uint8_t setExRegWrite;
    uint8_t setWbRegWrite;
    uint8_t setExMemRead;
    uint8_t setIsBranch;
    uint8_t setIsJump;
    uint8_t setBranchTaken;
    uint8_t setDivBusy;
    uint8_t setMemBusy;
    uint8_t setInstNotReady;

    // Expected outputs
    uint8_t getHoldFlag;
    uint8_t getFlushIfId;
    uint8_t getFlushIdEx;
    uint8_t getForwardASel;
    uint8_t getForwardBSel;
    uint8_t getCanForwardA;
    uint8_t getCanForwardB;
};

class HazardDetectionTestGenerator : public SimObject {
private:
    HazardDetection& dut;
    std::vector<TestVector> testVectors;
    size_t passed;
    size_t failed;

    void runTests();

public:
    HazardDetectionTestGenerator(const HazardDetectionTestGeneratorParams& params);

    void startup() override;
};

} // namespace gem5

#endif // __GENERATORS_HAZARD_DETECTION_TEST_GENERATOR_HH__
