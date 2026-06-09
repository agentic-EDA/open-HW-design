#include "generators/riscv/alu_test/alu_test_generator.hh"

#include "base/trace.hh"
#include "debug/AluTestGenerator.hh"
#include "sim/sim_exit.hh"

namespace gem5 {

AluTestGenerator::AluTestGenerator(const AluTestGeneratorParams &params) :
    SimObject(params),
    dut(*params.dut),
    passed(0),
    failed(0) {
    DPRINTF(AluTestGenerator, "AluTestGenerator created\n");
}

void
AluTestGenerator::runTests() {
    TestVector testVectors[] = {
        {5, 3, 0, 0, 0, 0, 0, 8, 0, 0, 0, "TP001_01: ADD基本加法: 5+3=8"},
        {0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, "TP001_02: ADD基本加法: 0+0=0"},
        {4294967295U, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, "TP001_03: ADD基本加法: 0xFFFFFFFF+1=0 (溢出)"},
        {2147483648U, 2147483648U, 0, 0, 0, 0, 0, 0, 1, 0, 0, "TP002_01: ADD溢出: 0x80000000+0x80000000=0x0"},
        {2147483647, 1, 0, 0, 0, 0, 0, 2147483648U, 0, 0, 0, "TP002_02: ADD溢出: 0x7FFFFFFF+1=0x80000000"},
        {10, 3, 1, 0, 0, 0, 0, 7, 0, 0, 0, "TP003_01: SUB基本减法: 10-3=7"},
        {5, 10, 1, 0, 0, 0, 0, 4294967291U, 0, 1, 1, "TP003_02: SUB基本减法: 5-10=-5"},
        {5, 5, 1, 0, 0, 0, 0, 0, 1, 0, 0, "TP003_03: SUB相等: 5-5=0"},
        {0, 1, 1, 0, 0, 0, 0, 4294967295U, 0, 1, 1, "TP004_01: SUB下溢: 0-1=0xFFFFFFFF"},
        {2147483648U, 1, 1, 0, 0, 0, 0, 2147483647, 0, 1, 0, "TP004_02: SUB下溢: 0x80000000-1=0x7FFFFFFF"},
        {3, 5, 2, 0, 0, 0, 0, 1, 0, 1, 1, "TP005_01: SLT: 有符号小于 3<5 为真"},
        {5, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, "TP005_02: SLT: 有符号小于 5<3 为假"},
        {4294967295U, 1, 2, 0, 0, 0, 0, 1, 0, 1, 0, "TP005_03: SLT: 有符号 -1<1 为真(负数小于正数)"},
        {1, 4294967295U, 2, 0, 0, 0, 0, 0, 0, 0, 1, "TP005_04: SLT: 有符号 1<-1 为假(正数大于负数)"},
        {5, 5, 2, 0, 0, 0, 0, 0, 1, 0, 0, "TP005_05: SLT: 有符号相等 5<5 为假"},
        {3, 5, 3, 0, 0, 0, 0, 1, 0, 1, 1, "TP006_01: SLTU: 无符号小于 3<5 为真"},
        {4294967295U, 1, 3, 0, 0, 0, 0, 0, 0, 1, 0, "TP006_02: SLTU: 无符号 -1=0xFFFFFFFF < 1 为假(无符号大数)"},
        {1, 4294967295U, 3, 0, 0, 0, 0, 1, 0, 0, 1, "TP006_03: SLTU: 无符号 1 < -1=0xFFFFFFFF 为真"},
        {5, 3, 4, 0, 0, 0, 0, 6, 0, 0, 0, "TP007_01: XOR: 5^3=6"},
        {4294967295U, 4294967295U, 4, 0, 0, 0, 0, 0, 1, 0, 0, "TP007_02: XOR: 0xFFFFFFFF^0xFFFFFFFF=0"},
        {0, 291, 4, 0, 0, 0, 0, 291, 0, 1, 1, "TP007_03: XOR: 0^0x123=0x123"},
        {5, 3, 5, 0, 0, 0, 0, 7, 0, 0, 0, "TP008_01: OR: 5|3=7"},
        {0, 0, 5, 0, 0, 0, 0, 0, 1, 0, 0, "TP008_02: OR: 0|0=0"},
        {5, 3, 6, 0, 0, 0, 0, 1, 0, 0, 0, "TP009_01: AND: 5&3=1"},
        {255, 15, 6, 0, 0, 0, 0, 15, 0, 0, 0, "TP009_02: AND: 0xFF&0x0F=0x0F"},
        {5, 2, 7, 0, 0, 0, 0, 20, 0, 0, 0, "TP010_01: SLL: 5<<2=20"},
        {1, 31, 7, 0, 0, 0, 0, 2147483648U, 0, 1, 1, "TP010_02: SLL: 1<<31=0x80000000"},
        {5, 0, 7, 0, 0, 0, 0, 5, 0, 0, 0, "TP011_01: SLL边界: 移位量=0, 5<<0=5"},
        {5, 32, 7, 0, 0, 0, 0, 5, 0, 1, 1, "TP011_02: SLL边界: 移位量=32, 只取低5位=0, 5<<0=5"},
        {20, 2, 8, 0, 0, 0, 0, 5, 0, 0, 0, "TP012_01: SRL: 20>>2=5"},
        {2147483648U, 31, 8, 0, 0, 0, 0, 1, 0, 1, 0, "TP012_02: SRL: 0x80000000>>31=1"},
        {20, 0, 8, 0, 0, 0, 0, 20, 0, 0, 0, "TP013_01: SRL边界: 移位量=0, 20>>0=20"},
        {20, 32, 8, 0, 0, 0, 0, 20, 0, 1, 1, "TP013_02: SRL边界: 移位量=32, 只取低5位=0, 20>>0=20"},
        {4294967288U, 2, 9, 0, 0, 0, 0, 4294967294U, 0, 1, 0, "TP014_01: SRA: -8>>2=-2 (0xFFFFFFF8>>2=0xFFFFFFFE)"},
        {8, 2, 9, 0, 0, 0, 0, 2, 0, 0, 0, "TP014_02: SRA: 8>>2=2"},
        {2147483648U, 31, 9, 0, 0, 0, 0, 4294967295U, 0, 1, 0, "TP015_01: SRA边界: 符号扩展 0x80000000>>31=0xFFFFFFFF(-1)"},
        {2147483648U, 0, 9, 0, 0, 0, 0, 2147483648U, 0, 1, 0, "TP015_02: SRA边界: 移位量=0, 0x80000000>>0=0x80000000"},
        {0, 3053977600U, 10, 0, 0, 0, 0, 3053977600U, 0, 0, 1, "TP016_01: LUI: operand_b=0x12345000, 结果=0x12345000"},
        {0, 0, 10, 0, 0, 0, 0, 0, 1, 0, 0, "TP016_02: LUI: operand_b=0, 结果=0"},
        {4096, 3053977600U, 11, 0, 0, 0, 0, 3053981696U, 0, 0, 1, "TP017_01: AUIPC: PC=0x1000, immediate=0x12345000, 结果=0x12346000"},
        {6, 7, 0, 1, 0, 0, 0, 42, 0, 1, 1, "TP018_01: MUL: 6*7=42, 低32位=42"},
        {4294967293U, 4, 0, 1, 0, 0, 0, 4294967284U, 0, 1, 0, "TP018_02: MUL: -3*4=-12, 低32位=0xFFFFFFF4"},
        {0, 100, 0, 1, 0, 0, 0, 0, 0, 1, 1, "TP018_03: MUL: 0*100=0"},
        {2147483648U, 2147483648U, 0, 1, 1, 0, 0, 1073741824U, 1, 0, 0, "TP019_01: MULH: 有符号 0x80000000 * 0x80000000, 高32位=0x40000000"},
        {3, 5, 0, 1, 1, 0, 0, 0, 0, 1, 1, "TP019_02: MULH: 有符号 3 * 5, 高32位=0"},
        {4294967295U, 2, 0, 1, 2, 0, 0, 4294967295U, 0, 1, 0, "TP020_01: MULHSU: 有符号 -1 * 无符号 2, 高32位=0xFFFFFFFF"},
        {3, 5, 0, 1, 2, 0, 0, 0, 0, 1, 1, "TP020_02: MULHSU: 有符号 3 * 无符号 5, 高32位=0"},
        {2147483648U, 2147483648U, 0, 1, 3, 0, 0, 1073741824U, 1, 0, 0, "TP021_01: MULHU: 无符号 0x80000000 * 0x80000000, 高32位=0x40000000"},
        {3, 5, 0, 1, 3, 0, 0, 0, 0, 1, 1, "TP021_02: MULHU: 无符号 3 * 5, 高32位=0"},
        {4294967295U, 4294967295U, 0, 1, 0, 0, 0, 1, 1, 0, 0, "TP022_01: 乘法边界: 0xFFFFFFFF * 0xFFFFFFFF (有符号-1*-1=1)"},
        {2147483647, 2147483647, 0, 1, 0, 0, 0, 1, 1, 0, 0, "TP022_02: 乘法边界: 0x7FFFFFFF * 0x7FFFFFFF, 低32位"},
        {10, 3, 0, 0, 0, 1, 0, 1, 0, 0, 0, "TP023_01: REM: 10/3=1 余数"},
        {4294967286U, 3, 0, 0, 0, 1, 0, 4294967295U, 0, 1, 0, "TP023_02: REM: -10/3=-3 余数(有符号 -10 rem 3 = -1)"},
        {10, 3, 0, 0, 0, 1, 1, 1, 0, 0, 0, "TP024_01: REMU: 10/3=1 余数(无符号)"},
        {4294967295U, 3, 0, 0, 0, 1, 1, 0, 0, 1, 0, "TP024_02: REMU: 0xFFFFFFFF/3 无符号余数"},
        {10, 0, 0, 0, 0, 1, 0, 4294967295U, 0, 0, 0, "TP025_01: REM除数为0: 10/0=-1(0xFFFFFFFF)"},
        {10, 0, 0, 0, 0, 1, 1, 10, 0, 0, 0, "TP025_02: REMU除数为0: 10/0=10"},
        {2147483648U, 4294967295U, 0, 0, 0, 1, 0, 2147483648U, 0, 1, 1, "TP026_01: REM边界: 0x80000000 / -1 = 0x80000000(结果不变)"},
        {4294967295U, 4294967295U, 0, 0, 0, 1, 0, 0, 1, 0, 0, "TP026_02: REM边界: -1 / -1 = 0"},
        {5, 5, 0, 0, 0, 0, 0, 10, 1, 0, 0, "TP027_01: branch_eq: 5==5 为真"},
        {5, 3, 0, 0, 0, 0, 0, 8, 0, 0, 0, "TP027_02: branch_eq: 5==3 为假"},
        {4294967295U, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, "TP028_01: branch_lt: 有符号 -1 < 1 为真"},
        {1, 4294967295U, 0, 0, 0, 0, 0, 0, 0, 0, 1, "TP028_02: branch_lt: 有符号 1 < -1 为假"},
        {1, 4294967295U, 0, 0, 0, 0, 0, 0, 0, 0, 1, "TP029_01: branch_ltu: 无符号 1 < 0xFFFFFFFF 为真"},
        {4294967295U, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, "TP029_02: branch_ltu: 无符号 0xFFFFFFFF < 1 为假"},
        {2, 3, 0, 0, 0, 0, 0, 5, 0, 1, 1, "TP030_01: 交叉: ADD后SLL, 2+3=5, 5<<1=10"},
        {5, 1, 7, 0, 0, 0, 0, 10, 0, 0, 0, "TP030_01b: 交叉: 接着SLL, 5<<1=10"},
        {4, 3, 0, 1, 0, 0, 0, 12, 0, 0, 0, "TP031_01: 交叉: MUL后SRL, 4*3=12, 12>>2=3"},
        {12, 2, 8, 0, 0, 0, 0, 3, 0, 0, 0, "TP031_01b: 交叉: 接着SRL, 12>>2=3"},
    };

    size_t numTests = sizeof(testVectors) / sizeof(testVectors[0]);

    DPRINTF(AluTestGenerator, "Starting ALU tests, total: %d\n", numTests);

    for (size_t i = 0; i < numTests; i++) {
        TestVector& vec = testVectors[i];

        DPRINTF(AluTestGenerator, "Running test %s\n", vec.description);

        dut.setOperandA(vec.operand_a);
        dut.setOperandB(vec.operand_b);
        dut.setAluOp(vec.alu_op);
        dut.setIsMul(vec.is_mul);
        dut.setMulOp(vec.mul_op);
        dut.setIsRem(vec.is_rem);
        dut.setRemUnsigned(vec.rem_unsigned);

        dut.process();

        uint32_t result = dut.getAluResult();
        uint8_t eq = dut.getBranchEq();
        uint8_t lt = dut.getBranchLt();
        uint8_t ltu = dut.getBranchLtu();

        bool test_passed = true;

        if (result != vec.expected_alu_result) {
            DPRINTF(AluTestGenerator, "  FAIL: alu_result mismatch: expected=0x%x, got=0x%x\n",
                    vec.expected_alu_result, result);
            test_passed = false;
        }

        if (eq != vec.expected_branch_eq) {
            DPRINTF(AluTestGenerator, "  FAIL: branch_eq mismatch: expected=%d, got=%d\n",
                    vec.expected_branch_eq, eq);
            test_passed = false;
        }

        if (lt != vec.expected_branch_lt) {
            DPRINTF(AluTestGenerator, "  FAIL: branch_lt mismatch: expected=%d, got=%d\n",
                    vec.expected_branch_lt, lt);
            test_passed = false;
        }

        if (ltu != vec.expected_branch_ltu) {
            DPRINTF(AluTestGenerator, "  FAIL: branch_ltu mismatch: expected=%d, got=%d\n",
                    vec.expected_branch_ltu, ltu);
            test_passed = false;
        }

        if (test_passed) {
            passed++;
            DPRINTF(AluTestGenerator, "  PASS\n");
        } else {
            failed++;
            DPRINTF(AluTestGenerator, "  TEST FAILED: %s\n", vec.description);
        }
    }

    DPRINTF(AluTestGenerator, "Test complete: passed=%d, failed=%d\n", passed, failed);

    if (failed > 0) {
        panic("Some ALU tests failed!");
    }

    exitSimLoop("AluTestGenerator complete");
}

void
AluTestGenerator::startup() {
    runTests();
}

} // namespace gem5
