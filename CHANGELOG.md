# 变更记录

## 未发布

- 通用检查器、回归及默认工具策略迁入 xgen-quality，CRC 只保留薄入口与配置。
- 生产 CRC API、版本与二进制实现保持不变。

## 0.1.0 本地试点

- 提供独立 CRC8 MAXIM 与 CRC16 MODBUS C11 组件及增量接口。
- 公开命名采用 xgcrc，提供独立 CMake 包和按算法选择的 targets。
- 保持来源 seed、NULL 和零长度行为，迁移 GoogleTest 行为测试。
- 接入 X-Gen 格式、严格 API 文档、静态分析、覆盖率与检查失败验证。

本版本号标识本地包基线，不代表已创建远端 release 或确认源码许可证。实际验证见 docs/implementation-log.md。
