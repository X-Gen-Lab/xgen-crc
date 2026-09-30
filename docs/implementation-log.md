# CRC 试点实施记录

本文件记录真实执行证据；本地检查、远端 CI、公开发布和目标板验证分别记录。

## 工作包与实现范围

实施日期：2026-10-01。工作包：S1 / LIB-CRC，本地分支为 `feat/independent-crc`。

- 输入是固定 core 提交与 roadmap 规范，不依赖工作区中未提交的 link 重构。
- 保持 CRC8 MAXIM / CRC16 MODBUS 的生产算法、seed、NULL、零长度及增量行为，迁入独立命名空间。
- 交付公开 C11 接口、按算法选择的源码/安装 targets、GoogleTest、质量入口、pre-commit 与 CI 配置。
- 新质量检查行为执行 TDD；原算法提取执行原行为基线和新包回归。
- 本次不切换 link，不删除 core，也不增加 CRC32、硬件后端或运行时组件系统。

实施顺序为来源验证、测试契约、源码提取、消费验证、工具正反例、记录与提交。可以通过固定原 core 提交恢复旧消费者；新仓库不重写来源历史。

## 来源基线

- core 来源提交：`dc5eb1167ba21de384e7a760a8586b87502b73b2`，工作区干净。
- 规范来源：roadmap `c018272a1bfa0a0ad0e811ceedcb22862640ed98`，工程规范 1.0.0，Nexus 格式基线。
- Windows / GCC 13.2.0 / CMake 4.2.3 / Ninja 1.12.1：在 `out/source-baseline` 构建原 `xgc_tests`，执行 `ctest --test-dir out/source-baseline -R '^xgc_crc$' --no-tests=error --output-on-failure`，1/1 通过。
- CRC 生产实现按保持行为的提取处理；未故意修改算法制造失败。

## 质量工具 TDD

首次 RED 命令：`python -m unittest discover -s tools/tests -v`。

14 个测试实际运行，因尚未实现的质量接口抛出 `NotImplementedError` 而失败，退出码 1。覆盖路径边界、文件选择、空测试集、跳过/缺失用例、失败退出和报告输入验证。执行时存在最小接口骨架；原始日志位于本地 `out/reports/quality-red.txt`。测试提交记录测试契约，不声称该骨架已作为生产实现完成。

首次测试契约已在 `b93910a9899b9c3ab1b6462ddedadef54bbf49f5` 提交。实现后通过，随后以 RED/GREEN 补充源码 HEAD、脏状态和配置 SHA256 报告，再补充失败、禁用、超时等回归。

真实负例还发现 Doxygen 对完全缺少文件文档的公开头可能静默忽略。新增回归先重现，再补充公开头与文件注释检查；覆盖率同步补充完整报告及缺失、空报告、陈旧报告的拒绝逻辑。这些缺陷修复的 RED 记录位于 `out/reports/quality-docs-coverage-red.txt`。

最终 `python -m unittest discover -s tools/tests -v` 为 **28/28 通过**。对应 GREEN 日志为 `out/reports/quality-docs-coverage-green.txt`；报告、源码和实际检查结果均未使用固定成功数据替代。

## Windows GCC 验证

环境：Windows、GCC/G++/gcov 13.2.0、CMake 4.2.3、Ninja 1.12.1、Python 3.12.10。GoogleTest 1.16.0 使用固定来源 `ff6133ab49b364a883a55ba75c39e520fea6245b`，单独显式构建安装到 `out/deps/gtest`。

| 检查 | 实际结果 |
| --- | --- |
| 原 core CRC 基线 | 1/1 通过 |
| GoogleTest 行为用例 | 12/12 通过；包含标准向量、全切分、seed、NULL、零长度、独立逐位参考与双流状态 |
| 独立消费 CTest | 10/10 通过；源码/安装的 CRC8、CRC16、双算法、可选/缺失/未知组件及不兼容版本 |
| C/C++ 头自包含 | 公开 CRC 头和生成版本头分别编译通过 |
| 纯 C Release | 配置和构建通过，不启用 C++ 或 GoogleTest |
| 最小符号检查 | 每个生产库仅导出本算法两个函数，无 core、堆或 GoogleTest 未定义符号 |
| Nexus 格式 | clang-format 19.1.5 通过；格式与 EditorConfig 快照逐字节等于规范模板 |
| 静态分析 | Cppcheck 2.21.0、clang-tidy 19.1.0 对真实生产编译数据库检查通过 |
| API 文档 | Doxygen 1.16.0 严格检查通过 |
| 覆盖率 | gcovr 8.3：行 17/17、函数 4/4、分支 8/8，分别为 100% |

覆盖率使用单独的 `out/coverage`，只有 `src/crc8.c` 与 `src/crc16.c` 的生产对象插桩。测试、GoogleTest、消费测试中的独立 Release/Debug 构建不进入分母；完整报告保存在 `out/reports/coverage.json`，汇总在 `coverage-summary.json`。

复现命令与依赖准备见 [贡献指南](../CONTRIBUTING.md)。JSON 报告还记录实际源码提交、脏状态、配置摘要、工具版本、执行命令、用例清单与退出码；实施中的报告会如实显示未提交工作区。

## Windows MSVC 验证

另使用 VS2022 `cl 19.40.33811.0` 与 Ninja，在 `out/msvc-gtest` 独立构建安装相同提交的 GoogleTest；依赖和测试程序均使用 Debug `/MDd`，没有复用 GCC 二进制。host preset 只覆盖构建目录与依赖前缀。

生产 C11、测试 C++17 构建和链接通过，最终完整 CTest **22/22 通过**，包括 12 个单元与 10 个集成测试。日志与 JUnit 位于 `out/msvc-logs/ctest-full-rerun.log`、`.xml`。

首轮曾有一项源码消费测试在 CMake `ShowIncludes/main.c` 写入时出现 `Permission denied`，当时 21/22 通过。保留首轮失败日志；该项单独重试与整轮复跑均通过，具体原因尚未确认，没有添加吞错或自动重试逻辑。

## 检查失败路径

在 `out` 下隔离的临时 Git/CMake 工程调用同一 runner 和真实工具，不改动 CRC 生产文件。下列 **9 项** 均返回退出码 1，报告为 `failed`，并核实对应诊断：

| 输入 | 预期阻断原因 |
| --- | --- |
| 不符合 Nexus 的 C 排版 | clang-format 格式错误 |
| 指定不存在的 formatter | 工具无法启动 |
| CTest 标签选不到用例 | 必需测试集为空 |
| CTest 用例全部跳过 | JUnit 包含 skipped |
| 空指针解引用的生产翻译单元 | Cppcheck `nullPointer` |
| 相同有缺陷的翻译单元 | clang-tidy `NullDereference` |
| 公开头无 Doxygen 文件注释 | 文件入口预检失败 |
| 函数缺参数文档 | Doxygen 参数未文档化 |
| 文档使用不存在的参数名 | Doxygen 参数名不匹配 |

原始报告与工具诊断保存于 `out/reports/negative-cli.json`。Doxygen 诊断在其 `warnings.log` 中，失败退出仍由 runner 向上传播。工具版本不匹配、超时、缺失用例、禁用用例及报告缺失另由 Python 回归测试覆盖。

pre-commit 4.3.0 已安装在本地仓库，配置使用显式准备的工具环境。`python -m pre_commit run --all-files` 的文本/配置与 Nexus C/C++ 格式两项均通过；`git diff --cached --check` 通过。

## 交付边界

- 本地版本为 0.1.0，没有创建远端、tag 或 release。
- Linux/MSVC 的 GitHub Actions 配置已提供；远端工作流尚未执行，不能据本地验证标记通过。
- 来源仓库缺少完整 LICENSE，授权确认留在正式发布前处理，不自行声明 MIT 或 SPDX。
- link 与 core 的现有消费者仍使用旧入口，S2 接入后才能移除活跃重复来源；本次未修改它们的实现。
- 尚未测量 ARM ELF、MCU 周期、ISR 时限或完整 Boot Flash/RAM；库无堆依赖不等于整机资源验收通过。
- xgen-quality 仍待从第二个组件的实际复用中提炼，当前 runner 仅随 CRC 试点维护。
