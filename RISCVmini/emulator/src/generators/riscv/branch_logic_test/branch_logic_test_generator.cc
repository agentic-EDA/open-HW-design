#include "generators/riscv/branch_logic_test/branch_logic_test_generator.hh"

#include "base/trace.hh"
#include "debug/BranchLogicTestGenerator.hh"
#include "sim/sim_exit.hh"

namespace gem5 {

const BranchLogicTestGenerator::TestVector
BranchLogicTestGenerator::test_vectors[] = {
    {"TP001_01", "BEQ: rs1==rs2, 分支成立", 100, 100, 0, 16, 1000, 1, 1016},
    {"TP001_02", "BEQ: rs1!=rs2, 分支不成立", 100, 200, 0, 16, 1000, 0, 1016},
    {"TP001_03", "BEQ: 零值相等", 0, 0, 0, 16, 1000, 1, 1016},
    {"TP002_01", "BNE: rs1!=rs2, 分支成立", 100, 200, 1, 16, 1000, 1, 1016},
    {"TP002_02", "BNE: rs1==rs2, 分支不成立", 100, 100, 1, 16, 1000, 0, 1016},
    {"TP003_01", "BLT: 有符号rs1<rs2, 分支成立(正数)", 100, 200, 4, 16, 1000, 1, 1016},
    {"TP003_02", "BLT: 有符号rs1>=rs2, 分支不成立(正数)", 200, 100, 4, 16, 1000, 0, 1016},
    {"TP003_03", "BLT: 有符号负数<正数", 4294967295U, 100, 4, 16, 1000, 1, 1016},
    {"TP003_04", "BLT: 有符号负数比较(负数<负数)", 4294967295U, 4294967294U, 4, 16, 1000, 0, 1016},
    {"TP004_01", "BGE: 有符号rs1>=rs2, 分支成立(正数)", 200, 100, 5, 16, 1000, 1, 1016},
    {"TP004_02", "BGE: 有符号rs1==rs2, 分支成立", 100, 100, 5, 16, 1000, 1, 1016},
    {"TP004_03", "BGE: 有符号正数<负数, 分支不成立", 100, 4294967295U, 5, 16, 1000, 1, 1016},
    {"TP005_01", "BLTU: 无符号rs1<rs2, 分支成立", 100, 200, 6, 16, 1000, 1, 1016},
    {"TP005_02", "BLTU: 无符号比较(大数<小数), 分支不成立", 200, 100, 6, 16, 1000, 0, 1016},
    {"TP005_03", "BLTU: 0xFFFFFFFF < 0, 分支不成立(无符号)", 4294967295U, 0, 6, 16, 1000, 0, 1016},
    {"TP005_04", "BLTU: 0 < 0xFFFFFFFF, 分支成立(无符号)", 0, 4294967295U, 6, 16, 1000, 1, 1016},
    {"TP006_01", "BGEU: 无符号rs1>=rs2, 分支成立", 200, 100, 7, 16, 1000, 1, 1016},
    {"TP006_02", "BGEU: 无符号rs1==rs2, 分支成立", 100, 100, 7, 16, 1000, 1, 1016},
    {"TP006_03", "BGEU: 0xFFFFFFFF >= 0, 分支成立(无符号)", 4294967295U, 0, 7, 16, 1000, 1, 1016},
    {"TP006_04", "BGEU: 0 >= 0xFFFFFFFF, 分支不成立(无符号)", 0, 4294967295U, 7, 16, 1000, 0, 1016},
    {"TP007_01", "BEQ边界: rs1=rs2=0x7FFFFFFF(最大正数)", 2147483647, 2147483647, 0, 16, 1000, 1, 1016},
    {"TP007_02", "BEQ边界: rs1=rs2=0x80000000(最小负数)", 2147483648U, 2147483648U, 0, 16, 1000, 1, 1016},
    {"TP007_03", "BEQ边界: rs1=rs2=0xFFFFFFFF(最大无符号)", 4294967295U, 4294967295U, 0, 16, 1000, 1, 1016},
    {"TP008_01", "BEQ边界: rs1=0, rs2=1", 0, 1, 0, 16, 1000, 0, 1016},
    {"TP008_02", "BEQ边界: rs1=0x7FFFFFFF, rs2=0x80000000", 2147483647, 2147483648U, 0, 16, 1000, 0, 1016},
    {"TP011_01", "BLT边界: rs1=-1, rs2=0 (有符号负数<零)", 4294967295U, 0, 4, 16, 1000, 1, 1016},
    {"TP011_02", "BLT边界: rs1=-1, rs2=-2 (有符号-1 > -2)", 4294967295U, 4294967294U, 4, 16, 1000, 0, 1016},
    {"TP011_03", "BLT边界: rs1=0x80000000(-2147483648), rs2=-1", 2147483648U, 4294967295U, 4, 16, 1000, 1, 1016},
    {"TP012_01", "BLT边界: rs1=0x7FFFFFFF(2147483647), rs2=负数", 2147483647, 4294967295U, 4, 16, 1000, 0, 1016},
    {"TP015_01", "BLTU边界: rs1=0, rs2=1", 0, 1, 6, 16, 1000, 1, 1016},
    {"TP015_02", "BLTU边界: rs1=0xFFFFFFFF, rs2=0", 4294967295U, 0, 6, 16, 1000, 0, 1016},
    {"TP017_01", "分支目标地址: 基本计算", 100, 100, 0, 100, 1000, 1, 1100},
    {"TP017_02", "分支目标地址: 负立即数(向后跳转)", 100, 100, 0, 4294967284U, 1000, 1, 988},
    {"TP017_03", "分支目标地址: PC=0", 100, 100, 0, 100, 0, 1, 100},
    {"TP018_01", "分支目标地址边界: PC+imm溢出", 100, 100, 0, 4294967295U, 4294967295U, 1, 4294967294U},
    {"TP018_02", "分支目标地址边界: 最大PC+正立即数", 100, 100, 0, 1, 4294967295U, 1, 0},
    {"TP019_01", "非法branch_op=2", 100, 100, 2, 16, 1000, 0, 1016},
    {"TP019_02", "非法branch_op=3", 100, 100, 3, 16, 1000, 0, 1016},
    {"TP019_03", "非法branch_op=8 (masked to 0=BEQ)", 100, 100, 8, 16, 1000, 1, 1016},
    {"TP021_01", "有符号比较: rs1=0, rs2=0, BLT", 0, 0, 4, 16, 1000, 0, 1016},
    {"TP021_02", "有符号比较: rs1=0, rs2=0, BGE", 0, 0, 5, 16, 1000, 1, 1016},
    {"TP022_01", "符号位: rs1=0x7FFFFFFF, rs2=0x80000000, BLT", 2147483647, 2147483648U, 4, 16, 1000, 0, 1016},
    {"TP022_02", "符号位: rs1=0x80000000, rs2=0x7FFFFFFF, BLTU", 2147483648U, 2147483647, 6, 16, 1000, 0, 1016},
    {"TP023_01", "立即数符号扩展: 正立即数", 100, 100, 0, 2147483648U, 1000, 1, 2147484648U},
    {"TP023_02", "立即数符号扩展: 负立即数(最高位为1)", 100, 100, 0, 3221225472U, 1000, 1, 3221226472U},
};

const size_t BranchLogicTestGenerator::num_test_vectors =
    sizeof(BranchLogicTestGenerator::test_vectors) / sizeof(BranchLogicTestGenerator::test_vectors[0]);

BranchLogicTestGenerator::BranchLogicTestGenerator(const BranchLogicTestGeneratorParams &params) :
    SimObject(params), dut(*params.dut), passed(0), failed(0) {
    DPRINTF(BranchLogicTestGenerator, "Created BranchLogicTestGenerator\n");
}

void
BranchLogicTestGenerator::runTests()
{
    DPRINTF(BranchLogicTestGenerator, "Starting %d tests\n", num_test_vectors);

    for (size_t i = 0; i < num_test_vectors; i++) {
        const TestVector& vec = test_vectors[i];

        DPRINTF(BranchLogicTestGenerator, "Running test %s: %s\n",
                vec.id, vec.desc);

        dut.setRs1Val(vec.rs1_val);
        dut.setRs2Val(vec.rs2_val);
        dut.setBranchOp(vec.branch_op);
        dut.setBranchImm(vec.branch_imm);
        dut.setCurrentPc(vec.current_pc);
        dut.process();

        uint32_t branch_taken = dut.getBranchTaken();
        uint32_t branch_target = dut.getBranchTarget();

        if (branch_taken == vec.expected_branch_taken &&
            branch_target == vec.expected_branch_target) {
            passed++;
            DPRINTF(BranchLogicTestGenerator, "Test %s PASSED\n", vec.id);
        } else {
            failed++;
            DPRINTF(BranchLogicTestGenerator,
                    "Test %s FAILED: expected taken=%d target=%u, got taken=%d target=%u\n",
                    vec.id, vec.expected_branch_taken, vec.expected_branch_target,
                    branch_taken, branch_target);
        }
    }

    DPRINTF(BranchLogicTestGenerator, "Tests complete: %d passed, %d failed\n",
            passed, failed);

    if (failed > 0) {
        panic("BranchLogicTestGenerator: %d tests failed!", failed);
    }

    exitSimLoop("BranchLogicTestGenerator complete");
}

void
BranchLogicTestGenerator::startup()
{
    runTests();
}

} // namespace gem5
