# ChEvidenceTestGenerator Python 配置说明

## Python SimObject 参数

### ChEvidenceTestGenerator

| 参数名 | Python Param 类型 | 说明 | 是否必需 |
|--------|-------------------|------|----------|
| dut | Param.ChEvidence | 被测试的 ChEvidence 模块实例 | 是 |

## 使用示例

```python
from m5.objects import *

root = Root(full_system=False)

# 创建被测试的模块
root.dut = ChEvidence()

# 创建测试器
root.tester = ChEvidenceTestGenerator()
root.tester.dut = root.dut

m5.instantiate()
exit_event = m5.simulate()
```

## 注意事项

- dut 参数必须指向一个已实例化的 ChEvidence 对象
- 测试器在 startup() 阶段自动运行所有测试向量
- 测试完成后会调用 exitSimLoop() 退出仿真
- 可通过 --debug-flags=ChEvidenceTestGenerator 查看测试进度
