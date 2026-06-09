#include "generators/ldpc/ch_evidence/ch_evidence.hh"

#include "base/trace.hh"
#include "debug/ChEvidence.hh"

namespace gem5 {

ChEvidence::ChEvidence(const ChEvidenceParams &params) : SimObject(params)
{
    received_packet_reg = 0;
    bit_error_log_prob_reg = 0;

    for (int i = 0; i < 10; ++i) {
        channel_evidence[i] = 0;
    }

    DPRINTF(ChEvidence, "ChEvidence created (combinational)\n");
}

void
ChEvidence::setReceivedPacket(uint16_t val)
{
    received_packet_reg = val;
    process();
}

void
ChEvidence::setBitErrorLogProb(int32_t val)
{
    bit_error_log_prob_reg = val;
    process();
}

int32_t
ChEvidence::getChannelEvidence(int index)
{
    return channel_evidence[index];
}

void
ChEvidence::process()
{
    for (int j = 0; j < 10; ++j) {
        if ((received_packet_reg >> j) & 1) {
            channel_evidence[j] = -bit_error_log_prob_reg;
        } else {
            channel_evidence[j] = bit_error_log_prob_reg;
        }
    }
}

} // namespace gem5
