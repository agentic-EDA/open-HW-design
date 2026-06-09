#ifndef __GENERATORS_LDPC_VAR_TO_CHECK_TEST_GENERATOR_HH__
#define __GENERATORS_LDPC_VAR_TO_CHECK_TEST_GENERATOR_HH__

#include "generators/ldpc/var_to_check/var_to_check.hh"
#include "params/VarToCheckTestGenerator.hh"
#include "sim/sim_object.hh"

#include <cstddef>
#include <cstdint>

namespace gem5 {

class VarToCheckTestGenerator : public SimObject {
  private:
    VarToCheck &dut;
    size_t passed;
    size_t failed;

    struct TestVector {
        int32_t inputs[4];
        int32_t expected_sum;
        const char* desc;
    };

    static const TestVector testVectors[];
    static constexpr size_t numVectors = 6;

    void runTests();

  public:
    VarToCheckTestGenerator(const VarToCheckTestGeneratorParams &p);
    void startup() override;
};

} // namespace gem5

#endif // __GENERATORS_LDPC_VAR_TO_CHECK_TEST_GENERATOR_HH__
