#include "generators/riscv/instruction_decoder/instruction_decoder.hh"

#include "base/trace.hh"
#include "debug/InstructionDecoder.hh"

namespace gem5 {

static const uint8_t OPCODE_R_M = 0x33;
static const uint8_t OPCODE_I_ALU = 0x13;
static const uint8_t OPCODE_I_LOAD = 0x03;
static const uint8_t OPCODE_JALR = 0x67;
static const uint8_t OPCODE_S = 0x23;
static const uint8_t OPCODE_B = 0x63;
static const uint8_t OPCODE_LUI = 0x37;
static const uint8_t OPCODE_AUIPC = 0x17;
static const uint8_t OPCODE_JAL = 0x6F;
static const uint8_t OPCODE_CSR = 0x73;
static const uint8_t OPCODE_FENCE = 0x0F;

static const uint8_t ALU_OP_ADD = 0x0;
static const uint8_t ALU_OP_SUB = 0x1;
static const uint8_t ALU_OP_SLT = 0x2;
static const uint8_t ALU_OP_SLTU = 0x3;
static const uint8_t ALU_OP_XOR = 0x4;
static const uint8_t ALU_OP_OR = 0x5;
static const uint8_t ALU_OP_AND = 0x6;
static const uint8_t ALU_OP_SLL = 0x7;
static const uint8_t ALU_OP_SRL = 0x8;
static const uint8_t ALU_OP_SRA = 0x9;
static const uint8_t ALU_OP_LUI = 0xA;
static const uint8_t ALU_OP_AUIPC = 0xB;

static const uint32_t INST_MRET = 0x30200073;
static const uint32_t INST_ECALL = 0x00000073;
static const uint32_t INST_EBREAK = 0x00100073;
static const uint32_t INST_NOP = 0x00000001;

InstructionDecoder::InstructionDecoder(const InstructionDecoderParams &params) :
    SimObject(params),
    instruction_reg(0),
    opcode_reg(0),
    funct3_reg(0),
    funct7_reg(0),
    rd_reg(0),
    rs1_reg(0),
    rs2_reg(0),
    imm_i_reg(0),
    imm_s_reg(0),
    imm_b_reg(0),
    imm_u_reg(0),
    imm_j_reg(0),
    is_r_type_reg(0),
    is_i_type_reg(0),
    is_s_type_reg(0),
    is_b_type_reg(0),
    is_u_type_reg(0),
    is_j_type_reg(0),
    is_jalr_reg(0),
    is_lui_reg(0),
    is_auipc_reg(0),
    is_load_reg(0),
    is_store_reg(0),
    is_alu_reg_reg(0),
    is_alu_imm_reg(0),
    alu_op_reg(0),
    mem_op_reg(0),
    reg_write_reg(0),
    mem_read_reg(0),
    mem_write_reg(0),
    is_branch_reg(0),
    branch_op_reg(0),
    is_csr_reg(0),
    csr_op_reg(0),
    csr_use_imm_reg(0),
    is_mul_reg(0),
    is_div_reg(0),
    is_mret_reg(0),
    is_ecall_reg(0),
    is_ebreak_reg(0),
    is_nop_reg(0),
    is_fence_reg(0)
{
    DPRINTF(InstructionDecoder, "InstructionDecoder object created\n");
}

uint32_t
InstructionDecoder::signExtend(uint32_t value, int bits)
{
    if ((value >> (bits - 1)) & 1) {
        return value | (~0U << bits);
    }
    return value & ((1U << bits) - 1);
}

uint32_t
InstructionDecoder::getITypeImm(uint32_t inst)
{
    return signExtend((inst >> 20) & 0xFFF, 12);
}

uint32_t
InstructionDecoder::getSTypeImm(uint32_t inst)
{
    uint32_t imm = ((inst >> 25) << 5) | ((inst >> 7) & 0x1F);
    return signExtend(imm, 12);
}

uint32_t
InstructionDecoder::getBTypeImm(uint32_t inst)
{
    uint32_t imm = ((inst >> 31) << 12) |
                   ((inst >> 7) & (1 << 11)) |
                   ((inst >> 20) & 0x7E0) |
                   ((inst >> 7) & 0x1E);
    return signExtend(imm, 13);
}

uint32_t
InstructionDecoder::getUTypeImm(uint32_t inst)
{
    return inst & 0xFFFFF000;
}

uint32_t
InstructionDecoder::getJTypeImm(uint32_t inst)
{
    uint32_t imm = ((inst >> 31) << 20) |
                   ((inst >> 12) & 0xFF000) |
                   ((inst >> 20) & (1 << 11)) |
                   ((inst >> 21) & 0x7FE);
    return signExtend(imm, 21);
}

uint8_t
InstructionDecoder::decodeAluOp(uint32_t inst, uint8_t opcode, uint8_t funct3, uint8_t funct7)
{
    if (opcode == OPCODE_LUI) {
        return ALU_OP_LUI;
    }
    if (opcode == OPCODE_AUIPC) {
        return ALU_OP_AUIPC;
    }
    if (opcode == OPCODE_R_M) {
        if (funct7 == 0x00) {
            if (funct3 == 0x0) return ALU_OP_ADD;
            if (funct3 == 0x1) return ALU_OP_SLL;
            if (funct3 == 0x2) return ALU_OP_SLT;
            if (funct3 == 0x3) return ALU_OP_SLTU;
            if (funct3 == 0x4) return ALU_OP_XOR;
            if (funct3 == 0x5) return ALU_OP_SRL;
            if (funct3 == 0x6) return ALU_OP_OR;
            if (funct3 == 0x7) return ALU_OP_AND;
        }
        if (funct7 == 0x20) {
            if (funct3 == 0x0) return ALU_OP_SUB;
            if (funct3 == 0x5) return ALU_OP_SRA;
        }
    }
    if (opcode == OPCODE_I_ALU) {
        if (funct3 == 0x0) return ALU_OP_ADD;
        if (funct3 == 0x1) {
            if ((inst >> 30) & 1) return ALU_OP_SRA;
            return ALU_OP_SLL;
        }
        if (funct3 == 0x2) return ALU_OP_SLT;
        if (funct3 == 0x3) return ALU_OP_SLTU;
        if (funct3 == 0x4) return ALU_OP_XOR;
        if (funct3 == 0x5) {
            if ((inst >> 30) & 1) return ALU_OP_SRA;
            return ALU_OP_SRL;
        }
        if (funct3 == 0x6) return ALU_OP_OR;
        if (funct3 == 0x7) return ALU_OP_AND;
    }
    return ALU_OP_ADD;
}

uint8_t
InstructionDecoder::decodeMemOp(uint8_t funct3)
{
    return funct3;
}

uint8_t
InstructionDecoder::decodeBranchOp(uint8_t funct3)
{
    return funct3;
}

uint8_t
InstructionDecoder::decodeCsrOp(uint8_t funct3)
{
    return funct3;
}

void
InstructionDecoder::process()
{
    uint32_t inst = instruction_reg;

    opcode_reg = inst & 0x7F;
    funct3_reg = (inst >> 12) & 0x7;
    funct7_reg = (inst >> 25) & 0x7F;
    rd_reg = (inst >> 7) & 0x1F;
    rs1_reg = (inst >> 15) & 0x1F;
    rs2_reg = (inst >> 20) & 0x1F;

    imm_i_reg = getITypeImm(inst);
    imm_s_reg = getSTypeImm(inst);
    imm_b_reg = getBTypeImm(inst);
    imm_u_reg = getUTypeImm(inst);
    imm_j_reg = getJTypeImm(inst);

    is_r_type_reg = (opcode_reg == OPCODE_R_M && ((funct7_reg >> 0) & 1) == 0) ? 1 : 0;
    is_i_type_reg = (opcode_reg == OPCODE_I_ALU) ? 1 : 0;
    is_s_type_reg = (opcode_reg == OPCODE_S) ? 1 : 0;
    is_b_type_reg = (opcode_reg == OPCODE_B) ? 1 : 0;
    is_u_type_reg = (opcode_reg == OPCODE_LUI || opcode_reg == OPCODE_AUIPC) ? 1 : 0;
    is_j_type_reg = (opcode_reg == OPCODE_JAL) ? 1 : 0;
    is_jalr_reg = (opcode_reg == OPCODE_JALR) ? 1 : 0;
    is_lui_reg = (opcode_reg == OPCODE_LUI) ? 1 : 0;
    is_auipc_reg = (opcode_reg == OPCODE_AUIPC) ? 1 : 0;
    is_load_reg = (opcode_reg == OPCODE_I_LOAD) ? 1 : 0;
    is_store_reg = (opcode_reg == OPCODE_S) ? 1 : 0;

    is_alu_reg_reg = is_r_type_reg;
    is_alu_imm_reg = is_i_type_reg;

    alu_op_reg = decodeAluOp(inst, opcode_reg, funct3_reg, funct7_reg);

    mem_op_reg = decodeMemOp(funct3_reg);

    is_branch_reg = (opcode_reg == OPCODE_B) ? 1 : 0;
    branch_op_reg = decodeBranchOp(funct3_reg);

    is_csr_reg = (opcode_reg == OPCODE_CSR) ? 1 : 0;
    csr_op_reg = decodeCsrOp(funct3_reg);
    csr_use_imm_reg = (funct3_reg == 0x5 || funct3_reg == 0x6 || funct3_reg == 0x7) ? 1 : 0;

    is_mul_reg = (opcode_reg == OPCODE_R_M && ((funct7_reg >> 0) & 1) == 1 &&
                  (funct3_reg <= 0x3)) ? 1 : 0;
    is_div_reg = (opcode_reg == OPCODE_R_M && ((funct7_reg >> 0) & 1) == 1 &&
                  (funct3_reg >= 0x4)) ? 1 : 0;

    is_mret_reg = (inst == INST_MRET) ? 1 : 0;
    is_ecall_reg = (inst == INST_ECALL) ? 1 : 0;
    is_ebreak_reg = (inst == INST_EBREAK) ? 1 : 0;
    is_nop_reg = (inst == INST_NOP) ? 1 : 0;
    is_fence_reg = (opcode_reg == OPCODE_FENCE) ? 1 : 0;

    if (opcode_reg == OPCODE_R_M || opcode_reg == OPCODE_I_ALU ||
        opcode_reg == OPCODE_LUI || opcode_reg == OPCODE_AUIPC ||
        opcode_reg == OPCODE_JAL || opcode_reg == OPCODE_JALR ||
        opcode_reg == OPCODE_I_LOAD || opcode_reg == OPCODE_CSR) {
        reg_write_reg = 1;
    } else {
        reg_write_reg = 0;
    }
    if (rd_reg == 0) {
        reg_write_reg = 0;
    }

    mem_read_reg = (opcode_reg == OPCODE_I_LOAD) ? 1 : 0;
    mem_write_reg = (opcode_reg == OPCODE_S) ? 1 : 0;

    DPRINTF(InstructionDecoder,
            "Decoded inst=0x%08x opcode=0x%02x funct3=0x%01x funct7=0x%02x\n",
            inst, opcode_reg, funct3_reg, funct7_reg);
}

void
InstructionDecoder::setInstruction(uint32_t val)
{
    instruction_reg = val;
}

uint8_t
InstructionDecoder::getOpcode()
{
    return opcode_reg;
}

uint8_t
InstructionDecoder::getFunct3()
{
    return funct3_reg;
}

uint8_t
InstructionDecoder::getFunct7()
{
    return funct7_reg;
}

uint8_t
InstructionDecoder::getRd()
{
    return rd_reg;
}

uint8_t
InstructionDecoder::getRs1()
{
    return rs1_reg;
}

uint8_t
InstructionDecoder::getRs2()
{
    return rs2_reg;
}

uint32_t
InstructionDecoder::getImmI()
{
    return imm_i_reg;
}

uint32_t
InstructionDecoder::getImmS()
{
    return imm_s_reg;
}

uint32_t
InstructionDecoder::getImmB()
{
    return imm_b_reg;
}

uint32_t
InstructionDecoder::getImmU()
{
    return imm_u_reg;
}

uint32_t
InstructionDecoder::getImmJ()
{
    return imm_j_reg;
}

uint8_t
InstructionDecoder::getIsRType()
{
    return is_r_type_reg;
}

uint8_t
InstructionDecoder::getIsIType()
{
    return is_i_type_reg;
}

uint8_t
InstructionDecoder::getIsSType()
{
    return is_s_type_reg;
}

uint8_t
InstructionDecoder::getIsBType()
{
    return is_b_type_reg;
}

uint8_t
InstructionDecoder::getIsUType()
{
    return is_u_type_reg;
}

uint8_t
InstructionDecoder::getIsJType()
{
    return is_j_type_reg;
}

uint8_t
InstructionDecoder::getIsJalr()
{
    return is_jalr_reg;
}

uint8_t
InstructionDecoder::getIsLui()
{
    return is_lui_reg;
}

uint8_t
InstructionDecoder::getIsAuipc()
{
    return is_auipc_reg;
}

uint8_t
InstructionDecoder::getIsLoad()
{
    return is_load_reg;
}

uint8_t
InstructionDecoder::getIsStore()
{
    return is_store_reg;
}

uint8_t
InstructionDecoder::getIsAluReg()
{
    return is_alu_reg_reg;
}

uint8_t
InstructionDecoder::getIsAluImm()
{
    return is_alu_imm_reg;
}

uint8_t
InstructionDecoder::getAluOp()
{
    return alu_op_reg;
}

uint8_t
InstructionDecoder::getMemOp()
{
    return mem_op_reg;
}

uint8_t
InstructionDecoder::getRegWrite()
{
    return reg_write_reg;
}

uint8_t
InstructionDecoder::getMemRead()
{
    return mem_read_reg;
}

uint8_t
InstructionDecoder::getMemWrite()
{
    return mem_write_reg;
}

uint8_t
InstructionDecoder::getIsBranch()
{
    return is_branch_reg;
}

uint8_t
InstructionDecoder::getBranchOp()
{
    return branch_op_reg;
}

uint8_t
InstructionDecoder::getIsCsr()
{
    return is_csr_reg;
}

uint8_t
InstructionDecoder::getCsrOp()
{
    return csr_op_reg;
}

uint8_t
InstructionDecoder::getCsrUseImm()
{
    return csr_use_imm_reg;
}

uint8_t
InstructionDecoder::getIsMul()
{
    return is_mul_reg;
}

uint8_t
InstructionDecoder::getIsDiv()
{
    return is_div_reg;
}

uint8_t
InstructionDecoder::getIsMret()
{
    return is_mret_reg;
}

uint8_t
InstructionDecoder::getIsEcall()
{
    return is_ecall_reg;
}

uint8_t
InstructionDecoder::getIsEbreak()
{
    return is_ebreak_reg;
}

uint8_t
InstructionDecoder::getIsNop()
{
    return is_nop_reg;
}

uint8_t
InstructionDecoder::getIsFence()
{
    return is_fence_reg;
}

} // namespace gem5
