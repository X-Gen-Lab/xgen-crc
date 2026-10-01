# 来源与授权记录

本仓库提取自 X-Gen-Lab/xgen-core 提交 `dc5eb1167ba21de384e7a760a8586b87502b73b2`。原 core 又记录来源为 xgen-link `a3b71e5` 的 CRC MAXIM/MODBUS 实现。原作者标记保留为 X-Gen Lab。

| 本仓库 | 来源 |
| --- | --- |
| src/crc8.c | core src/crc8.c |
| src/crc16.c | core src/crc16.c |
| include/xgen/crc/crc.h | core include/xgc/crc.h 的接口与行为，改为独立前缀并补全契约 |
| tests/unit | core tests/test_core.c 的 CRC 测试向量、分段与参考算法验证，迁入 GoogleTest |

生产算法保持不变；公开符号由 `xgc_` 改为 `xgcrc_`，包含路径变为 `xgen/crc/`。独立构建、包导出、质量工具和接入文档为本次新增。

来源的 PROVENANCE.md 明确记录：原工作区没有顶层 LICENSE，提取未声明新授权。本仓库沿用这一事实，尚未确定完整许可证；不复制 roadmap 的 MIT 许可，不虚构 SPDX 标识。正式公开发布之前需由仓库所有者确认授权和版权声明。

## 开发依赖与配置

- GoogleTest 是单独准备的主机测试依赖，采用上游 1.16.0 提交 `ff6133ab49b364a883a55ba75c39e520fea6245b`；保留其上游许可，不将其源码或库安装进本包。
- Nexus 格式与 EditorConfig 快照来源于 `7a203266082ad1f655b6686713b2b7950ba31ec6`，经 roadmap 工程规范 1.0.0 固定。
- 规范初始本地提交：`c018272a1bfa0a0ad0e811ceedcb22862640ed98`。初次配置仅调整标题和验证工具版本注释，格式选项保持一致；后续空行增补另记，不将当前配置等同于初始来源的全部选项。CI 按本仓质量要求维护。

## 2026-10-01 格式配置增补

按 xgen-roadmap 尚未发布的工程规范 1.0.0 local 增补 C-020、C-021、DOC-013，受控 `.clang-format` 增加 `SeparateDefinitionBlocks: Always`、`KeepEmptyLines` 三项 false 和 `LineEnding: LF`，保留 `MaxEmptyLinesToKeep: 1`。当前格式模板 SHA-256 为 `ffdb331b03ae4f6c5f75ee54d4afaa6d4741f5ac3ec57f55b0f04ad8ec396a2a`；`.editorconfig` 的来源不变。

本轮源码迁移仅调整空行和 LF，不改变代码行为；来源、作者和许可证事实保持。共享质量工具的当前固定来源见 [tools/quality.json](tools/quality.json)，格式检查边界见 [规范采用记录](docs/standards.md)。这份记录不代表远端 CI 已执行。
