# 仓库工作约定

- 始终使用简体中文与用户沟通；先阅读 CONTRIBUTING.md、docs/standards.md 和 PROVENANCE.md。
- 使用 Nexus 格式和反斜杠 Doxygen，根 .clang-format 为受控配置，不改回旧 core 风格。
- 生产代码保持 C11、无堆、无全局可变状态；不引入 core、status、RTOS 或测试框架依赖。
- CRC 保持来源算法及 NULL/零长度/seed 语义。新增行为或修复执行 TDD；纯迁移先建立来源基线。
- C/C++ 主机测试使用 GoogleTest；工具自身使用 Python unittest。不要复制被测算法来计算期望值。
- 依赖获取是显式准备操作，配置、编译和测试不隐式下载。
- 本地检查、远端 CI、发布与硬件结果分别记录；当前没有明确源码许可证，不擅自套用 MIT 等授权。
- 本地、pre-commit 和 CI 调用 tools/quality.py 薄入口；规则、回归与工具版本只在 xgen-quality 维护，工具失败不得被吞掉。
