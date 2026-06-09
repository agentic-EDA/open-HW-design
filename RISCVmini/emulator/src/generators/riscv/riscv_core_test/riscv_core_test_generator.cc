#include "generators/riscv/riscv_core_test/riscv_core_test_generator.hh"

#include "base/trace.hh"
#include "debug/RiscvCoreTestGenerator.hh"
#include "sim/sim_exit.hh"

namespace gem5 {

RiscvCoreTestGenerator::RiscvCoreTestGenerator(
    const RiscvCoreTestGeneratorParams& params)
    : SimObject(params), dut(*params.dut), currentTest(0), currentCycle(0), passed(0), failed(0),
      event([this]{runNextCycle();}, name()) {
    testSequences.push_back({"TP001", "复位测试 - 验证复位后PC=0，输出信号正确初始化", {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1048723, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    }});
    testSequences.push_back({"TP002", "ADD指令测试 - ADDI x1, x0, 5; ADD x2, x1, x1 (结果x2=10)", {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 5243027, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1081779, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1048595, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    }});
    testSequences.push_back({"TP015", "BEQ指令测试 - 相等时分支成立", {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 5243027, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 5243155, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 2130275, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1048595, 1, 0, 0, 0, 0, 0, 0, 0, 4, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    }});
    testSequences.push_back({"TP019", "JAL指令测试 - 跳转并链接", {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 12583151, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1048595, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    }});
    testSequences.push_back({"TP021", "Load-Use冒险测试 - LW x1, 0(x0); ADD x2, x1, x1", {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 8323, 1, 305419896, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1081779, 1, 305419896, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1048595, 1, 305419896, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 2, 1, 0, 0},
    }});
    testSequences.push_back({"TP042", "指令总线未就绪测试 - 验证指令总线未就绪时流水线暂停", {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1048723, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1048723, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1048851, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    }});
    testSequences.push_back({"TP010_TP013", "LW和SW指令测试 - SW x1, 0(x0); LW x2, 0(x0)", {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 5243027, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1056803, 1, 0, 0, 1, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 8451, 1, 305419896, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    }});
    testSequences.push_back({"TP035", "ECALL指令测试 - 触发异常，进入异常处理器", {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 115, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1048595, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    }});
    testSequences.push_back({"TP037", "Timer0外部中断测试 - 验证外部中断响应", {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1048723, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1048595, 1, 0, 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    }});
    testSequences.push_back({"TP006", "移位指令测试 - SLLI x1, x0, 8; SRLI x1, x1, 4; SRAI x1, x1, 2", {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 8388755, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1069331, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1069459, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    }});
    testSequences.push_back({"TP041", "零寄存器(x0)测试 - 验证x0恒为0，写入无效", {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 5242899, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1048595, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    }});
    testSequences.push_back({"TP051", "MUL指令测试 - ADDI x1, x0, 10; MUL x1, x1, x1", {
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 10485939, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 33591475, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
        {1, 19, 1, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    }});
}

void
RiscvCoreTestGenerator::startup()
{
    DPRINTF(RiscvCoreTestGenerator, "Starting riscv_core tests, total %d sequences\n", (int)testSequences.size());
    schedule(event, 1);
}

void
RiscvCoreTestGenerator::runNextCycle()
{
    if (currentTest >= testSequences.size()) {
        DPRINTF(RiscvCoreTestGenerator, "Tests complete: %d passed, %d failed, %d total\n",
                (int)passed, (int)failed, (int)testSequences.size());
        if (failed > 0) {
            panic("RiscvCoreTestGenerator: %d tests failed!", (int)failed);
        }
        exitSimLoop("RiscvCoreTestGenerator complete");
        return;
    }
    
    TestSequence& seq = testSequences[currentTest];
    
    if (currentCycle >= seq.cycles.size()) {
        DPRINTF(RiscvCoreTestGenerator, "Test %s PASSED %s\n", seq.id, seq.desc);
        passed++;
        currentTest++;
        currentCycle = 0;
        schedule(event, curTick() + 1);
        return;
    }
    
    TestCycle& cyc = seq.cycles[currentCycle];

    dut.setRst(cyc.rst);
    dut.setRibInstI(cyc.rib_inst_i);
    dut.setRibInstReadyI(cyc.rib_inst_ready_i);
    dut.setRibDataI(cyc.rib_data_i);
    dut.setRibDataReadyI(cyc.rib_data_ready_i);
    dut.setRibDataAckI(cyc.rib_data_ack_i);
    dut.setClintIntI(cyc.clint_int_i);
    dut.setClintIntAssertI(cyc.clint_int_assert_i);
    dut.setDivResultI(cyc.div_result_i);
    dut.setDivReadyI(cyc.div_ready_i);

    dut.runCycle();

    bool pass = true;
    if (dut.getRibInstAddrO() != cyc.rib_inst_addr_o) { pass = false; }
    if (dut.getRibInstReqO() != cyc.rib_inst_req_o) { pass = false; }
    if (dut.getRibInstEnO() != cyc.rib_inst_en_o) { pass = false; }
    if (dut.getRibDataAddrO() != cyc.rib_data_addr_o) { pass = false; }
    if (dut.getRibDataWdataO() != cyc.rib_data_wdata_o) { pass = false; }
    if (dut.getRibDataWeO() != cyc.rib_data_we_o) { pass = false; }
    if (dut.getRibDataReO() != cyc.rib_data_re_o) { pass = false; }
    if (dut.getRibDataEnO() != cyc.rib_data_en_o) { pass = false; }
    if (dut.getRibDataSizeO() != cyc.rib_data_size_o) { pass = false; }
    if (dut.getRibDataReqO() != cyc.rib_data_req_o) { pass = false; }
    if (dut.getClintIntO() != cyc.clint_int_o) { pass = false; }
    if (dut.getClintIntAssertO() != cyc.clint_int_assert_o) { pass = false; }
    
    if (!pass) {
        failed++;
        DPRINTF(RiscvCoreTestGenerator, "Test %s cycle %d FAILED  %s\n", seq.id, (int)currentCycle,seq.desc);
        panic("RiscvCoreTestGenerator: Test %s cycle %d failed! %s\n", seq.id, (int)currentCycle,seq.desc);
        return;
    }

    currentCycle++;
    schedule(event, curTick() + 1);
}

} // namespace gem5
