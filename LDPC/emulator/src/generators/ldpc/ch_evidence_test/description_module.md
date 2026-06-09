# ChEvidenceTestGenerator 模块功能描述

## 模块功能概述

ChEvidenceTestGenerator 是用于测试 ChEvidence 组合逻辑模块的测试生成器。该测试器读取预定义的测试向量，通过调用 ChEvidence 模块的接口设置输入、执行计算并验证输出结果。

## 端口列表及说明

测试模块不直接定义端口，而是通过 Param 引用被测试的 ChEvidence 模块（dut），并调用其以下接口：

### 输入设置接口（通过 dut.setXXX 调用）
- setReceivedPacket(uint16_t val) - 设置接收到的数据包（10位）
- setBitErrorLogProb(int32_t val) - 设置比特错误对数概率

### 输出获取接口（通过 dut.getXXX 调用）
- getChannelEvidence(int index) - 获取第 index 个通道证据值（index: 0-9）

## 参数列表及说明

| 参数名 | 类型 | 说明 |
|--------|------|------|
| dut | Param.ChEvidence | 被测试的设备（ChEvidence 模块实例） |

## 内部信号说明

测试模块内部无状态寄存器，测试向量从 JSON 文件读取：
- 路径：src/generators/ldpc/ch_evidence/test_vectors.json
- 格式：包含 test_vectors 数组，每个元素包含 id、group、inputs 和 expected 字段

## 测试流程

1. startup() 被调用时直接执行 runTests()
2. 读取 test_vectors.json
3. 遍历每个测试向量：
   - 调用 dut.setReceivedPacket() 和 dut.setBitErrorLogProb() 设置输入
   - 调用 dut.process() 执行组合逻辑计算
   - 调用 dut.getChannelEvidence(i) 获取10个输出值
   - 与 expected 值比较，不一致则 failed 计数加1
4. 所有测试完成后，若有失败则 panic，否则 exitSimLoop
