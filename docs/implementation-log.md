# CRC 试点实施记录

本文件记录真实执行证据；本地检查、远端 CI、公开发布和目标板验证分别记录。

## 来源基线

- core 来源提交：`dc5eb1167ba21de384e7a760a8586b87502b73b2`，工作区干净。
- 规范来源：roadmap `c018272a1bfa0a0ad0e811ceedcb22862640ed98`，工程规范 1.0.0，Nexus 格式基线。
- Windows / GCC 13.2.0 / CMake 4.2.3 / Ninja 1.12.1：在 `out/source-baseline` 构建原 `xgc_tests`，执行 `ctest --test-dir out/source-baseline -R '^xgc_crc$' --no-tests=error --output-on-failure`，1/1 通过。
- CRC 生产实现按保持行为的提取处理；未故意修改算法制造失败。

## 质量工具 TDD

首次 RED 命令：`python -m unittest discover -s tools/tests -v`。

14 个测试实际运行，因尚未实现的质量接口抛出 `NotImplementedError` 而失败，退出码 1。覆盖路径边界、文件选择、空测试集、跳过/缺失用例、失败退出和报告输入验证。执行时存在最小接口骨架；原始日志位于本地 `out/reports/quality-red.txt`。测试提交记录测试契约，不声称该骨架已作为生产实现完成。

后续 GREEN、组件、安装消费、覆盖率和负例结果在实际执行后补充。
