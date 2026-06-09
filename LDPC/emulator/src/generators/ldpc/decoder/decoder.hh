#ifndef __GENERATORS_LDPC_DECODER_HH__
#define __GENERATORS_LDPC_DECODER_HH__

#include <array>
#include <cstdint>
#include <vector>

#include "generators/ldpc/belief/belief.hh"
#include "generators/ldpc/ch_evidence/ch_evidence.hh"
#include "generators/ldpc/check_to_var/check_to_var.hh"
#include "generators/ldpc/var_to_check/var_to_check.hh"
#include "params/Decoder.hh"
#include "sim/eventq.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class DecoderParams;

class Decoder : public SimObject
{
  private:
    void processEvent();
    EventFunctionWrapper event;

    ChEvidence *ch_evidence;
    std::vector<VarToCheck *> var_to_check;
    std::vector<CheckToVar *> check_to_var;
    std::vector<Belief *> belief;

    uint16_t received_packet_reg;
    int32_t bit_error_log_prob_reg;

    std::array<int32_t, 10> channel_belief;
    uint16_t corrected_seq_val;
    int32_t c2v_old[25];
    int32_t c2v_new[25];

    void computeChannelEvidence();
    void computeNewVtoCm();
    void computeNewCtoVm();
    void computeChannelBelief();

  public:
    Decoder(const DecoderParams &params);

    void startup() override;

    void setReceivedPacket(uint16_t val);
    void setBitErrorLogProb(int32_t val);

    int32_t getChannelBelief(int index);
    uint16_t getCorrectedSeq();

    void processCycle();
};

} // namespace gem5

#endif // __GENERATORS_LDPC_DECODER_HH__
