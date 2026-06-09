#include "generators/riscv/forwarding_unit_test/forwarding_unit_test_generator.hh"
#include "base/logging.hh"
#include "base/trace.hh"
#include "debug/ForwardingUnitTestGenerator.hh"
#include "sim/sim_exit.hh"
#include <cstdio>

namespace gem5 {

ForwardingUnitTestGenerator::ForwardingUnitTestGenerator(
    const ForwardingUnitTestGeneratorParams &params)
    : SimObject(params), dut(*params.dut), passed(0), failed(0)
{
    testVectors = {
        {5, 6, 7, 8, 0, 0, 0, 0, 0, "TP001_01: 无前递：所有写使能关闭"},
        {5, 6, 7, 8, 1, 1, 0, 0, 0, "TP001_02: 无前递：寄存器不匹配"},
        {5, 6, 5, 8, 1, 0, 0, 1, 0, "TP002_01: EX/MEM前递操作数A：ex_rd==id_rs1"},
        {5, 6, 5, 5, 1, 1, 0, 1, 0, "TP002_02: EX/MEM前递操作数A：ex_rd==id_rs1且mem也匹配但EX优先"},
        {10, 11, 10, 12, 1, 0, 1, 1, 0, "TP002_03: EX/MEM前递操作数A：ex_rd==id_rs1且ex_rd!=0"},
        {5, 6, 6, 8, 1, 0, 0, 0, 1, "TP003_01: EX/MEM前递操作数B：ex_rd==id_rs2"},
        {5, 6, 6, 6, 1, 1, 0, 0, 1, "TP003_02: EX/MEM前递操作数B：ex_rd==id_rs2且mem也匹配但EX优先"},
        {5, 6, 7, 5, 0, 1, 0, 2, 0, "TP004_01: MEM/WB前递操作数A：mem_rd==id_rs1且EX不匹配"},
        {10, 11, 10, 10, 0, 1, 0, 2, 0, "TP004_02: MEM/WB前递操作数A：mem_rd==id_rs1且ex_reg_write=0"},
        {5, 6, 7, 6, 0, 1, 0, 0, 2, "TP005_01: MEM/WB前递操作数B：mem_rd==id_rs2且EX不匹配"},
        {5, 10, 10, 10, 0, 1, 0, 0, 2, "TP005_02: MEM/WB前递操作数B：mem_rd==id_rs2且ex_reg_write=0"},
        {5, 6, 5, 5, 1, 1, 0, 1, 0, "TP006_01: EX/MEM优先于MEM/WB（操作数A）：两者都匹配"},
        {5, 6, 5, 5, 1, 1, 1, 1, 0, "TP006_02: EX/MEM优先于MEM/WB（操作数A）：不同寄存器但都匹配对应操作数"},
        {5, 6, 6, 6, 1, 1, 0, 0, 1, "TP007_01: EX/MEM优先于MEM/WB（操作数B）：两者都匹配"},
        {5, 6, 6, 6, 1, 1, 1, 0, 1, "TP007_02: EX/MEM优先于MEM/WB（操作数B）：ex_rd和mem_rd不同但都匹配id_rs2"},
        {5, 6, 0, 8, 1, 0, 0, 0, 0, "TP008_01: x0不参与前递：ex_rd=0，即使ex_reg_write=1且不匹配"},
        {0, 6, 0, 8, 1, 0, 0, 0, 0, "TP008_02: x0不参与前递：ex_rd=0且ex_rd==id_rs1（id_rs1=0）"},
        {5, 6, 7, 0, 0, 1, 0, 0, 0, "TP009_01: x0不参与前递：mem_rd=0，即使mem_reg_write=1"},
        {0, 6, 7, 0, 0, 1, 0, 0, 0, "TP009_02: x0不参与前递：mem_rd=0且mem_rd==id_rs1（id_rs1=0）"},
        {0, 6, 0, 0, 1, 1, 0, 0, 0, "TP010_01: id_rs1=0查询x0：不应触发任何前递"},
        {0, 6, 0, 8, 1, 0, 0, 0, 0, "TP010_02: id_rs1=0查询x0：即使ex_rd=0也匹配但不触发"},
        {5, 0, 0, 0, 1, 1, 0, 0, 0, "TP011_01: id_rs2=0查询x0：不应触发任何前递"},
        {5, 0, 7, 0, 0, 1, 0, 0, 0, "TP011_02: id_rs2=0查询x0：即使mem_rd=0也匹配但不触发"},
        {5, 6, 5, 8, 0, 0, 0, 0, 0, "TP012_01: ex_reg_write=0时无EX/MEM前递：即使ex_rd==id_rs1"},
        {5, 6, 5, 5, 0, 1, 0, 2, 0, "TP012_02: ex_reg_write=0时无EX/MEM前递：但MEM/WB可以前递"},
        {5, 6, 7, 5, 0, 0, 0, 0, 0, "TP013_01: mem_reg_write=0时无MEM/WB前递：即使mem_rd==id_rs1"},
        {5, 6, 5, 5, 1, 0, 0, 1, 0, "TP013_02: mem_reg_write=0时无MEM/WB前递：但EX/MEM可以前递"},
        {5, 6, 5, 6, 1, 1, 0, 1, 2, "TP014_01: 操作数A和B同时前递（不同源）：A从EX，B从MEM"},
        {5, 6, 6, 5, 1, 1, 0, 2, 1, "TP014_02: 操作数A和B同时前递（不同源）：A从MEM，B从EX"},
        {5, 5, 5, 8, 1, 0, 0, 1, 1, "TP015_01: 操作数A和B都从EX/MEM前递：id_rs1==id_rs2==ex_rd"},
        {5, 5, 5, 5, 1, 1, 1, 1, 1, "TP015_02: 操作数A和B都从EX/MEM前递：id_rs1==id_rs2==ex_rd且mem也匹配但EX优先"},
        {5, 5, 7, 5, 0, 1, 0, 2, 2, "TP016_01: 操作数A和B都从MEM/WB前递：id_rs1==id_rs2==mem_rd"},
        {5, 5, 5, 5, 0, 1, 0, 2, 2, "TP016_02: 操作数A和B都从MEM/WB前递：id_rs1==id_rs2==mem_rd且ex_reg_write=0"},
        {5, 5, 5, 8, 1, 0, 0, 1, 1, "TP017_01: id_rs1等于id_rs2：两个操作数相同寄存器"},
        {5, 5, 6, 7, 1, 1, 0, 0, 0, "TP017_02: id_rs1等于id_rs2：两个操作数相同且无前递"},
        {5, 6, 5, 5, 1, 1, 0, 1, 0, "TP018_01: ex_rd等于mem_rd且都匹配操作数A：EX优先"},
        {5, 6, 6, 6, 1, 1, 0, 0, 1, "TP018_02: ex_rd等于mem_rd且都匹配操作数B：EX优先"},
        {5, 6, 5, 8, 1, 0, 1, 1, 0, "TP019_01: Load指令标志不影响前递：ex_mem_read=1但前递仍然工作"},
        {5, 6, 5, 5, 1, 1, 1, 1, 0, "TP019_02: Load指令标志不影响前递：ex_mem_read=1且MEM也匹配但EX优先"},
        {0, 0, 0, 0, 1, 1, 0, 0, 0, "TP020_01: 寄存器地址边界：最小值0（x0）"},
        {31, 31, 31, 31, 1, 1, 0, 1, 1, "TP020_02: 寄存器地址边界：最大值31"},
        {31, 31, 30, 31, 0, 1, 0, 2, 2, "TP020_03: 寄存器地址边界：31从MEM/WB前递"},
        {5, 6, 7, 5, 1, 1, 0, 2, 0, "TP021_01: MEM/WB前递作为备选：EX不匹配但MEM匹配操作数A"},
        {5, 6, 7, 6, 1, 1, 0, 0, 2, "TP021_02: MEM/WB前递作为备选：EX不匹配但MEM匹配操作数B"},
        {5, 6, 5, 8, 1, 0, 0, 1, 0, "TP022_01: 部分匹配：仅操作数A有EX前递，B无"},
        {5, 6, 7, 5, 0, 1, 0, 2, 0, "TP022_02: 部分匹配：仅操作数A有MEM前递，B无"},
        {5, 6, 6, 8, 1, 0, 0, 0, 1, "TP023_01: 部分匹配：仅操作数B有EX前递，A无"},
        {5, 6, 7, 6, 0, 1, 0, 0, 2, "TP023_02: 部分匹配：仅操作数B有MEM前递，A无"},
        {5, 6, 7, 8, 1, 1, 0, 0, 0, "TP024_01: ex_rd和mem_rd都不为0但都不匹配id_rs1/id_rs2"},
        {5, 6, 6, 5, 0, 0, 0, 0, 0, "TP024_02: ex_rd和mem_rd都不为0但都不匹配：交叉情况"},
        {5, 6, 5, 6, 0, 0, 1, 0, 0, "TP024_03: ex_rd和mem_rd都不为0但都不匹配：都匹配但写使能关闭"},
    };
}

void
ForwardingUnitTestGenerator::runTests()
{
    DPRINTF(ForwardingUnitTestGenerator, "Starting %zu tests\n", testVectors.size());

    for (size_t i = 0; i < testVectors.size(); i++) {
        const TestVector& vec = testVectors[i];

        dut.setIdRs1(vec.id_rs1);
        dut.setIdRs2(vec.id_rs2);
        dut.setExRd(vec.ex_rd);
        dut.setMemRd(vec.mem_rd);
        dut.setExRegWrite(vec.ex_reg_write);
        dut.setMemRegWrite(vec.mem_reg_write);
        dut.setExMemRead(vec.ex_mem_read);

        dut.process();

        uint8_t forward_a_sel = dut.getForwardASel();
        uint8_t forward_b_sel = dut.getForwardBSel();

        bool pass = true;
        if (forward_a_sel != vec.expected_forward_a_sel) {
            printf("FAIL: %s\n", vec.desc);
            printf("  forward_a_sel: expected=%u, got=%u\n",
                   vec.expected_forward_a_sel, forward_a_sel);
            pass = false;
        }
        if (forward_b_sel != vec.expected_forward_b_sel) {
            printf("FAIL: %s\n", vec.desc);
            printf("  forward_b_sel: expected=%u, got=%u\n",
                   vec.expected_forward_b_sel, forward_b_sel);
            pass = false;
        }

        if (pass) {
            passed++;
            DPRINTF(ForwardingUnitTestGenerator, "PASS: %s\n", vec.desc);
        } else {
            failed++;
        }
    }

    printf("\n========== Test Results ==========\n");
    printf("Total: %zu, Passed: %zu, Failed: %zu\n",
           testVectors.size(), passed, failed);
    printf("==================================\n");

    if (failed > 0) {
        panic("Some tests failed!");
    }

    exitSimLoop("ForwardingUnitTestGenerator complete");
}

void
ForwardingUnitTestGenerator::startup()
{
    runTests();
}

} // namespace gem5
