#ifndef __GENERATORS_LDPC_CH_EVIDENCE_TEST_GENERATOR_HH__
#define __GENERATORS_LDPC_CH_EVIDENCE_TEST_GENERATOR_HH__

#include "generators/ldpc/ch_evidence/ch_evidence.hh"
#include "params/ChEvidenceTestGenerator.hh"
#include "sim/sim_object.hh"

#include <cstddef>
#include <cstdint>

namespace gem5 {

class ChEvidenceTestGenerator : public SimObject {
  private:
    ChEvidence &dut;
    size_t passed;
    size_t failed;

    struct TestVector {
        uint16_t received_packet;
        int32_t bit_error_log_prob;
        int32_t expected_evidence[10];
        const char* desc;
    };

    static const TestVector testVectors[];
    static constexpr size_t numVectors = 5;

    void runTests();

  public:
    ChEvidenceTestGenerator(const ChEvidenceTestGeneratorParams &p);
    void startup() override;
};

} // namespace gem5

#endif // __GENERATORS_LDPC_CH_EVIDENCE_TEST_GENERATOR_HH__
