# xgen-crc

独立的 C11 CRC8 MAXIM 与 CRC16 MODBUS 组件。提供一次计算和增量更新，不使用堆、RTOS、core 或其他 X-Gen 组件。当前包版本为 0.1.0，本地试点；未配置远端或公开发布。

| 算法 | 公开 target | 初值 | 反射多项式 | 最终异或 |
| --- | --- | --- | --- | --- |
| CRC8 MAXIM | `xgcrc::crc8` | `0x00` | `0x8c`（正常形式 `0x31`） | `0` |
| CRC16 MODBUS | `xgcrc::crc16` | `0xffff` | `0xa001`（正常形式 `0x8005`） | `0` |

两个算法独立编译、导出和消费。公开头为 `xgen/crc/crc.h`，版本头为 `xgen/crc/version.h`。校验值以整数返回，线上编码的字节顺序由消费者决定。

## 契约

- 输入只读，不保存指针；调用同步完成，工作量与输入长度成正比。
- `len == 0` 时不读取数据，返回原 seed。
- 为保持来源行为，`data == NULL` 时无论长度多少都返回原 seed；一次计算返回相应初值。
- 非空指针必须指向至少 `len` 个可读字节。并发调用没有共享可变状态，但调用者不能同时修改输入范围。
- 不增加运行时算法注册、CRC32 或硬件后端。已有实现来源与授权状态见 [PROVENANCE.md](PROVENANCE.md)。

## 纯 C 构建和消费

需要 CMake 3.24 或更新版本、C11 编译器，preset 使用 Ninja。生产构建默认关闭测试，不要求 C++ 编译器或 GoogleTest。

```sh
cmake --preset release
cmake --build --preset release
cmake --install out/release --prefix out/install
```

安装消费者：

```cmake
find_package(xgen_crc 0.1 CONFIG REQUIRED COMPONENTS crc8)
target_link_libraries(my_app PRIVATE xgcrc::crc8)
```

配置消费者时通过 `CMAKE_PREFIX_PATH` 指向实际安装前缀。请求 `crc8` 不引入 `crc16`，请求不可用或未知的必需组件失败。源码消费可以显式 `add_subdirectory` 后链接相同 target；不要读取另一个仓库的私有目录。

只构建一种算法时使用 `XGCRC_BUILD_CRC8` / `XGCRC_BUILD_CRC16` 开关；至少启用一种。当前完整主机测试集要求同时启用两种。API 的独立前缀替代原 `xgc_`，不是旧包的二进制兼容包装。

## 主机测试

先按 [贡献指南](CONTRIBUTING.md) 显式准备固定 GoogleTest 包和检查工具，再执行：

```sh
cmake --preset host
cmake --build --preset host
python tools/quality.py test --build-dir out/host
```

测试以 C++17 和 GoogleTest 编译，链接真实 C11 生产 target。覆盖 CRC 向量、分段、空输入、任意 seed 与独立参考算法，并检查 C/C++ 头文件、源码消费和安装包消费。测试框架不进入生产导出包。

质量入口、覆盖率、失败验证与版本基线见 [贡献指南](CONTRIBUTING.md)、[规范采用](docs/standards.md) 和 [实施记录](docs/implementation-log.md)。GitHub Actions 配置不代表远端已经执行；本地结果与未执行事项分别记录。
