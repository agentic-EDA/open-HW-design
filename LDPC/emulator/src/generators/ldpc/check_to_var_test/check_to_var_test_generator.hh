#ifndef __GENERATORS_LDPC_CHECK_TO_VAR_TEST_GENERATOR_HH__
#define __GENERATORS_LDPC_CHECK_TO_VAR_TEST_GENERATOR_HH__

#include "generators/ldpc/check_to_var/check_to_var.hh"
#include "params/CheckToVarTestGenerator.hh"
#include "sim/sim_object.hh"

#include <cstddef>
#include <cstdint>

namespace gem5 {

class CheckToVarTestGenerator : public SimObject {
  private:
    CheckToVar &dut;
    size_t passed;
    size_t failed;

    struct TestVector {
        int32_t inputs[4];
        int32_t expected_output;
        const char* desc;
    };

    static const TestVector testVectors[];
    static constexpr size_t numVectors = 6;

    void runTests();

  public:
    CheckToVarTestGenerator(const CheckToVarTestGeneratorParams &p);
    void startup() override;
};

} // namespace gem5

#endif // __GENERATORS_LDPC_CHECK_TO_VAR_TEST_GENERATOR_HH__
