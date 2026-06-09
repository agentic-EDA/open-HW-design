#ifndef __GENERATORS_ALU_ALU_HH__
#define __GENERATORS_ALU_ALU_HH__

#include <cstdint>

#include "params/Alu.hh"
#include "sim/sim_object.hh"

namespace gem5 {

class AluParams;

class Alu : public SimObject {
  private:
    uint32_t operand_a_reg;
    uint32_t operand_b_reg;
    uint8_t alu_op_reg;
    uint8_t is_mul_reg;
    uint8_t mul_op_reg;
    uint8_t is_rem_reg;
    uint8_t rem_unsigned_reg;

    uint32_t alu_result_val;
    uint8_t branch_eq_val;
    uint8_t branch_lt_val;
    uint8_t branch_ltu_val;

  public:
    Alu(const AluParams &params);

    void setOperandA(uint32_t val);
    void setOperandB(uint32_t val);
    void setAluOp(uint8_t val);
    void setIsMul(uint8_t val);
    void setMulOp(uint8_t val);
    void setIsRem(uint8_t val);
    void setRemUnsigned(uint8_t val);

    uint32_t getAluResult();
    uint8_t getBranchEq();
    uint8_t getBranchLt();
    uint8_t getBranchLtu();

    void process();
};

} // namespace gem5

#endif // __GENERATORS_ALU_ALU_HH__
