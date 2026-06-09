#ifndef __GENERATORS_LDPC_BELIEF_TEST_GENERATOR_HH__
#define __GENERATORS_LDPC_BELIEF_TEST_GENERATOR_HH__

#include "generators/ldpc/belief/belief.hh"
#include "params/BeliefTestGenerator.hh"
#include "sim/sim_object.hh"

#include <cstddef>
#include <cstdint>

namespace gem5 {

class BeliefTestGenerator : public SimObject {
  private:
    Belief &dut;
    size_t passed;
    size_t failed;

    struct TestVector {
        int32_t inputs[4];
        int32_t expected_belief;
        uint32_t expected_corrected_bit;
        const char* desc;
    };

    static const TestVector testVectors[];
    static constexpr size_t numVectors = 6;

    void runTests();

  public:
    BeliefTestGenerator(const BeliefTestGeneratorParams &p);
    void startup() override;
};

} // namespace gem5

#endif // __GENERATORS_LDPC_BELIEF_TEST_GENERATOR_HH__
