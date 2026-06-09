#include "generators/riscv/alu/alu.hh"

#include "base/trace.hh"
#include "debug/Alu.hh"

namespace gem5 {

Alu::Alu(const AluParams &params) : SimObject(params)
{
    operand_a_reg = 0;
    operand_b_reg = 0;
    alu_op_reg = 0;
    is_mul_reg = 0;
    mul_op_reg = 0;
    is_rem_reg = 0;
    rem_unsigned_reg = 0;

    alu_result_val = 0;
    branch_eq_val = 0;
    branch_lt_val = 0;
    branch_ltu_val = 0;

    DPRINTF(Alu, "Alu created (combinational)\n");
}

void
Alu::setOperandA(uint32_t val)
{
    operand_a_reg = val;
    process();
}

void
Alu::setOperandB(uint32_t val)
{
    operand_b_reg = val;
    process();
}

void
Alu::setAluOp(uint8_t val)
{
    alu_op_reg = val;
    process();
}

void
Alu::setIsMul(uint8_t val)
{
    is_mul_reg = val;
    process();
}

void
Alu::setMulOp(uint8_t val)
{
    mul_op_reg = val;
    process();
}

void
Alu::setIsRem(uint8_t val)
{
    is_rem_reg = val;
    process();
}

void
Alu::setRemUnsigned(uint8_t val)
{
    rem_unsigned_reg = val;
    process();
}

uint32_t
Alu::getAluResult()
{
    return alu_result_val;
}

uint8_t
Alu::getBranchEq()
{
    return branch_eq_val;
}

uint8_t
Alu::getBranchLt()
{
    return branch_lt_val;
}

uint8_t
Alu::getBranchLtu()
{
    return branch_ltu_val;
}

void
Alu::process()
{
    int32_t signed_a = static_cast<int32_t>(operand_a_reg);
    int32_t signed_b = static_cast<int32_t>(operand_b_reg);

    branch_eq_val = (operand_a_reg == operand_b_reg) ? 1 : 0;
    branch_lt_val = (signed_a < signed_b) ? 1 : 0;
    branch_ltu_val = (operand_a_reg < operand_b_reg) ? 1 : 0;

    if (is_mul_reg) {
        int64_t full_product;
        if (mul_op_reg == 0) {
            full_product = static_cast<int64_t>(signed_a) * static_cast<int64_t>(signed_b);
            alu_result_val = static_cast<uint32_t>(full_product & 0xFFFFFFFF);
        } else if (mul_op_reg == 1) {
            full_product = static_cast<int64_t>(signed_a) * static_cast<int64_t>(signed_b);
            alu_result_val = static_cast<uint32_t>((full_product >> 32) & 0xFFFFFFFF);
        } else if (mul_op_reg == 2) {
            full_product = static_cast<int64_t>(signed_a) * static_cast<int64_t>(operand_b_reg);
            alu_result_val = static_cast<uint32_t>((full_product >> 32) & 0xFFFFFFFF);
        } else if (mul_op_reg == 3) {
            full_product = static_cast<int64_t>(operand_a_reg) * static_cast<int64_t>(operand_b_reg);
            alu_result_val = static_cast<uint32_t>((full_product >> 32) & 0xFFFFFFFF);
        } else {
            alu_result_val = 0;
        }
    } else if (is_rem_reg) {
        if (rem_unsigned_reg) {
            if (operand_b_reg == 0) {
                alu_result_val = operand_a_reg;
            } else {
                alu_result_val = operand_a_reg % operand_b_reg;
            }
        } else {
            if (signed_b == 0) {
                alu_result_val = 0xFFFFFFFF;
            } else {
                alu_result_val = static_cast<uint32_t>(signed_a % signed_b);
            }
        }
    } else {
        if (alu_op_reg == 0) {
            alu_result_val = operand_a_reg + operand_b_reg;
        } else if (alu_op_reg == 1) {
            alu_result_val = operand_a_reg - operand_b_reg;
        } else if (alu_op_reg == 2) {
            alu_result_val = (signed_a < signed_b) ? 1 : 0;
        } else if (alu_op_reg == 3) {
            alu_result_val = (operand_a_reg < operand_b_reg) ? 1 : 0;
        } else if (alu_op_reg == 4) {
            alu_result_val = operand_a_reg ^ operand_b_reg;
        } else if (alu_op_reg == 5) {
            alu_result_val = operand_a_reg | operand_b_reg;
        } else if (alu_op_reg == 6) {
            alu_result_val = operand_a_reg & operand_b_reg;
        } else if (alu_op_reg == 7) {
            uint8_t shift = operand_b_reg & 0x1F;
            alu_result_val = operand_a_reg << shift;
        } else if (alu_op_reg == 8) {
            uint8_t shift = operand_b_reg & 0x1F;
            alu_result_val = operand_a_reg >> shift;
        } else if (alu_op_reg == 9) {
            uint8_t shift = operand_b_reg & 0x1F;
            alu_result_val = static_cast<uint32_t>(signed_a >> shift);
        } else if (alu_op_reg == 10) {
            alu_result_val = operand_b_reg;
        } else if (alu_op_reg == 11) {
            alu_result_val = operand_a_reg + operand_b_reg;
        } else {
            alu_result_val = 0;
        }
    }
}

} // namespace gem5
