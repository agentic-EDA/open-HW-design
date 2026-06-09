#include "generators/ldpc/decoder/decoder.hh"

#include "base/trace.hh"
#include "debug/LdpcDecoder.hh"

namespace gem5 {

Decoder::Decoder(const DecoderParams &params)
    : SimObject(params),
      event([this] { processCycle(); }, name()),
      ch_evidence(params.ch_evidence),
      var_to_check(params.var_to_check),
      check_to_var(params.check_to_var),
      belief(params.belief),
      received_packet_reg(0),
      bit_error_log_prob_reg(0),
      corrected_seq_val(0)
{
    for (int i = 0; i < 10; ++i) {
        channel_belief[i] = 0;
    }
    for (int i = 0; i < 25; i++) {
        c2v_old[i] = 0;
        c2v_new[i] = 0;
    }
    DPRINTF(LdpcDecoder, "Decoder created (sequential)\n");
}

void Decoder::startup() {}

void Decoder::processCycle()
{
    computeChannelEvidence();
    computeNewVtoCm();
    computeNewCtoVm();
    for (int i = 0; i < 25; i++) {
        c2v_old[i] = c2v_new[i];
    }
    computeChannelBelief();
}

void Decoder::setReceivedPacket(uint16_t val) { received_packet_reg = val; }
void Decoder::setBitErrorLogProb(int32_t val) { bit_error_log_prob_reg = val; }
int32_t Decoder::getChannelBelief(int index) { return channel_belief[index]; }
uint16_t Decoder::getCorrectedSeq() { return corrected_seq_val; }

static void setV2CInput(VarToCheck *v2c, int32_t msg0, int32_t msg1, int32_t msg2, int32_t msg3)
{
    uint64_t low = (static_cast<uint64_t>(static_cast<uint32_t>(msg1)) << 32)
                 | static_cast<uint64_t>(static_cast<uint32_t>(msg0));
    uint64_t high = (static_cast<uint64_t>(static_cast<uint32_t>(msg3)) << 32)
                  | static_cast<uint64_t>(static_cast<uint32_t>(msg2));
    v2c->setNeighbourChecksLow(low);
    v2c->setNeighbourChecksHigh(high);
    v2c->process();
}

void Decoder::computeChannelEvidence()
{
    ch_evidence->setReceivedPacket(received_packet_reg);
    ch_evidence->setBitErrorLogProb(bit_error_log_prob_reg);
    ch_evidence->process();
}

void Decoder::computeNewVtoCm()
{
    int32_t ce[10];
    for (int i = 0; i < 10; i++) ce[i] = ch_evidence->getChannelEvidence(i);

    // v0: c0(边0), c3(边1)
    setV2CInput(var_to_check[0],  ce[0], c2v_old[1],  0,         0);
    setV2CInput(var_to_check[1],  ce[0], c2v_old[0],  0,         0);
    // v1: c1(边2), c4(边3)
    setV2CInput(var_to_check[2],  ce[1], c2v_old[3],  0,         0);
    setV2CInput(var_to_check[3],  ce[1], c2v_old[2],  0,         0);
    // v2: c0(边4), c2(边5)
    setV2CInput(var_to_check[4],  ce[2], c2v_old[5],  0,         0);
    setV2CInput(var_to_check[5],  ce[2], c2v_old[4],  0,         0);
    // v3: c1(边6), c3(边7)
    setV2CInput(var_to_check[6],  ce[3], c2v_old[7],  0,         0);
    setV2CInput(var_to_check[7],  ce[3], c2v_old[6],  0,         0);
    // v4: c0(边8), c2(边9)
    setV2CInput(var_to_check[8],  ce[4], c2v_old[9],  0,         0);
    setV2CInput(var_to_check[9],  ce[4], c2v_old[8],  0,         0);
    // v5: c0(边10), c2(边11), c4(边12)
    setV2CInput(var_to_check[10], ce[5], c2v_old[11], c2v_old[12], 0);
    setV2CInput(var_to_check[11], ce[5], c2v_old[10], c2v_old[12], 0);
    setV2CInput(var_to_check[12], ce[5], c2v_old[10], c2v_old[11], 0);
    // v6: c1(边13), c2(边14), c4(边15)
    setV2CInput(var_to_check[13], ce[6], c2v_old[14], c2v_old[15], 0);
    setV2CInput(var_to_check[14], ce[6], c2v_old[13], c2v_old[15], 0);
    setV2CInput(var_to_check[15], ce[6], c2v_old[13], c2v_old[14], 0);
    // v7: c0(边16), c3(边17), c4(边18)
    setV2CInput(var_to_check[16], ce[7], c2v_old[17], c2v_old[18], 0);
    setV2CInput(var_to_check[17], ce[7], c2v_old[16], c2v_old[18], 0);
    setV2CInput(var_to_check[18], ce[7], c2v_old[16], c2v_old[17], 0);
    // v8: c1(边19), c3(边20), c4(边21)
    setV2CInput(var_to_check[19], ce[8], c2v_old[20], c2v_old[21], 0);
    setV2CInput(var_to_check[20], ce[8], c2v_old[19], c2v_old[21], 0);
    setV2CInput(var_to_check[21], ce[8], c2v_old[19], c2v_old[20], 0);
    // v9: c1(边22), c2(边23), c3(边24)
    setV2CInput(var_to_check[22], ce[9], c2v_old[23], c2v_old[24], 0);
    setV2CInput(var_to_check[23], ce[9], c2v_old[22], c2v_old[24], 0);
    setV2CInput(var_to_check[24], ce[9], c2v_old[22], c2v_old[23], 0);
}

void Decoder::computeNewCtoVm()
{
    int32_t vtoc[25];
    for (int i = 0; i < 25; i++) vtoc[i] = var_to_check[i]->getVarToCheckMessage();

    auto minSum5 = [&](int out[5], int e0, int e1, int e2, int e3, int e4) {
        int32_t v[5] = {vtoc[e0], vtoc[e1], vtoc[e2], vtoc[e3], vtoc[e4]};
        for (int j = 0; j < 5; j++) {
            int32_t min_abs = -1, sign = 0;
            bool first = true;
            for (int k = 0; k < 5; k++) {
                if (k == j) continue;
                int32_t s = (v[k] >> 31) & 1, a = std::abs(v[k]);
                if (first) { min_abs = a; sign = s; first = false; }
                else { sign ^= s; if (a < min_abs) min_abs = a; }
            }
            out[j] = sign ? -min_abs : min_abs;
        }
    };

    int out5[5];
    // c0: v0(0), v2(4), v4(8), v5(10), v7(16)
    minSum5(out5, 0, 4, 8, 10, 16);
    c2v_new[0] = out5[0]; c2v_new[4] = out5[1]; c2v_new[8] = out5[2];
    c2v_new[10] = out5[3]; c2v_new[16] = out5[4];
    // c1: v1(2), v3(6), v6(13), v8(19), v9(22)
    minSum5(out5, 2, 6, 13, 19, 22);
    c2v_new[2] = out5[0]; c2v_new[6] = out5[1]; c2v_new[13] = out5[2];
    c2v_new[19] = out5[3]; c2v_new[22] = out5[4];
    // c2: v2(5), v4(9), v5(11), v6(14), v9(23)
    minSum5(out5, 5, 9, 11, 14, 23);
    c2v_new[5] = out5[0]; c2v_new[9] = out5[1]; c2v_new[11] = out5[2];
    c2v_new[14] = out5[3]; c2v_new[23] = out5[4];
    // c3: v0(1), v3(7), v7(17), v8(20), v9(24)
    minSum5(out5, 1, 7, 17, 20, 24);
    c2v_new[1] = out5[0]; c2v_new[7] = out5[1]; c2v_new[17] = out5[2];
    c2v_new[20] = out5[3]; c2v_new[24] = out5[4];
    // c4: v1(3), v5(12), v6(15), v7(18), v8(21)
    minSum5(out5, 3, 12, 15, 18, 21);
    c2v_new[3] = out5[0]; c2v_new[12] = out5[1]; c2v_new[15] = out5[2];
    c2v_new[18] = out5[3]; c2v_new[21] = out5[4];
}

void Decoder::computeChannelBelief()
{
    int32_t ce[10];
    for (int i = 0; i < 10; i++) ce[i] = ch_evidence->getChannelEvidence(i);

    uint16_t corrected = 0;

    auto be = [&](int bi, int ce_i, int e0, int e1, int e2 = -1) {
        int32_t sum = ce[bi] + c2v_new[e0] + c2v_new[e1];
        if (e2 >= 0) sum += c2v_new[e2];
        channel_belief[bi] = sum;
        corrected |= ((sum > 0) ? 0 : 1) << bi;
    };

    be(0, 0, 0, 1);        // v0: c0(0), c3(1)
    be(1, 1, 2, 3);        // v1: c1(2), c4(3)
    be(2, 2, 4, 5);        // v2: c0(4), c2(5)
    be(3, 3, 6, 7);        // v3: c1(6), c3(7)
    be(4, 4, 8, 9);        // v4: c0(8), c2(9)
    be(5, 5, 10, 11, 12);  // v5: c0(10), c2(11), c4(12)
    be(6, 6, 13, 14, 15);  // v6: c1(13), c2(14), c4(15)
    be(7, 7, 16, 17, 18);  // v7: c0(16), c3(17), c4(18)
    be(8, 8, 19, 20, 21);  // v8: c1(19), c3(20), c4(21)
    be(9, 9, 22, 23, 24);  // v9: c1(22), c2(23), c3(24)

    corrected_seq_val = corrected;
}

} // namespace gem5
