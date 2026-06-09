#ifndef __GENERATORS_RISCV_CORE_TEST_GENERATOR_HH__
#define __GENERATORS_RISCV_CORE_TEST_GENERATOR_HH__

#include <cstdint>
#include <vector>

#include "generators/riscv/riscv_core/riscv_core.hh"
#include "params/RiscvCoreTestGenerator.hh"
#include "sim/eventq.hh"
#include "sim/sim_object.hh"

namespace gem5 {

struct TestCycle {
    uint8_t rst;
    uint32_t rib_inst_i;
    uint8_t rib_inst_ready_i;
    uint32_t rib_data_i;
    uint8_t rib_data_ready_i;
    uint8_t rib_data_ack_i;
    uint8_t clint_int_i;
    uint8_t clint_int_assert_i;
    uint32_t div_result_i;
    uint8_t div_ready_i;
    uint32_t rib_inst_addr_o;
    uint8_t rib_inst_req_o;
    uint8_t rib_inst_en_o;
    uint32_t rib_data_addr_o;
    uint32_t rib_data_wdata_o;
    uint8_t rib_data_we_o;
    uint8_t rib_data_re_o;
    uint8_t rib_data_en_o;
    uint8_t rib_data_size_o;
    uint8_t rib_data_req_o;
    uint8_t clint_int_o;
    uint8_t clint_int_assert_o;
};

struct TestSequence {
    const char* id;
    const char* desc;
    std::vector<TestCycle> cycles;
};

class RiscvCoreTestGenerator : public SimObject {
  private:
    RiscvCore& dut;
    std::vector<TestSequence> testSequences;
    size_t currentTest;
    size_t currentCycle;
    size_t passed;
    size_t failed;
    
    void runNextCycle();
    EventFunctionWrapper event;

  public:
    RiscvCoreTestGenerator(const RiscvCoreTestGeneratorParams& params);
    void startup() override;
};

} // namespace gem5

#endif // __GENERATORS_RISCV_CORE_TEST_GENERATOR_HH__
