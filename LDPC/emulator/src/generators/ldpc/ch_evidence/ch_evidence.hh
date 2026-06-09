#ifndef __GENERATORS_LDPC_CH_EVIDENCE_HH__
#define __GENERATORS_LDPC_CH_EVIDENCE_HH__

#include <cstdint>

#include "params/ChEvidence.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class ChEvidenceParams;

class ChEvidence : public SimObject {
  private:
    uint16_t received_packet_reg;
    int32_t bit_error_log_prob_reg;

    int32_t channel_evidence[10];

  public:
    ChEvidence(const ChEvidenceParams &params);

    void setReceivedPacket(uint16_t val);
    void setBitErrorLogProb(int32_t val);

    int32_t getChannelEvidence(int index);

    void process();
};

} // namespace gem5

#endif // __GENERATORS_LDPC_CH_EVIDENCE_HH__
