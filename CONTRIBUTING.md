# 贡献指南

先阅读 [规范采用](docs/standards.md) 和 [来源记录](PROVENANCE.md)。生产行为遵循 C11，主机单元测试遵循 C++17 / GoogleTest。新增行为和修复执行 TDD；纯提取保留原行为基线，格式迁移与算法修改分开审阅。

## 显式准备依赖

生产构建无需 GoogleTest。主机测试固定使用 GoogleTest 1.16.0，来源为 `https://github.com/google/googletest.git` 的提交 `ff6133ab49b364a883a55ba75c39e520fea6245b`。可使用同一版本、架构和编译运行库的已安装包；默认 preset 查找 `out/deps/gtest`。

以下是从上游显式获取和准备测试依赖的命令，当前目录为仓库根目录。已有源码可以直接跳过获取，但需要核实提交和未提交修改。

```sh
git clone --no-checkout https://github.com/google/googletest.git out/deps/gtest-src
git -C out/deps/gtest-src checkout --detach ff6133ab49b364a883a55ba75c39e520fea6245b
cmake -S out/deps/gtest-src -B out/deps/gtest-build -G Ninja -DCMAKE_BUILD_TYPE=Debug -DBUILD_GMOCK=OFF -DINSTALL_GTEST=ON -Dgtest_force_shared_crt=ON
```

在仓库根目录执行构建和安装，显式将包安装到本地前缀：

```sh
cmake --build out/deps/gtest-build
cmake --install out/deps/gtest-build --prefix out/deps/gtest
```

GoogleTest 和测试程序必须使用兼容的编译器、架构、配置及 C++ 运行库。Windows 可在 MSVC 开发终端使用 Ninja，或对依赖和本库同时显式设置 GCC/G++。改变工具链时使用新的构建目录；不要复用不兼容 CMake 缓存。

Python 工具准备：

```sh
python -m venv out/venv
```

激活环境：PowerShell 执行 `./out/venv/Scripts/Activate.ps1`，POSIX shell 执行 `. out/venv/bin/activate`。激活后：

```sh
python -m pip install /path/to/xgen_quality-0.1.0-py3-none-any.whl
python -m pre_commit install
```

将上例 wheel 路径替换为显式准备的实际文件；不要求工作区有固定兄弟目录，也不会从公网索引猜测同名包。包来源固定于 `tools/quality.json` 的 `quality_source.revision`，版本为 0.1.0。可从该提交的 xgen-quality 仓库按其贡献指南构建 wheel；组件只保留薄入口，不再维护检查器副本。

此处是显式依赖安装阶段。wheel 元数据固定 Python 工具依赖；离线安装时先准备依赖 wheelhouse，再用 `--no-index --find-links` 安装。之后 CMake 配置、编译和检查不联网、不自动升级。Cppcheck 2.21.0 与 Doxygen 1.16.0 另行准备；CI 从固定上游提交构建这两项工具。

## 统一检查入口

```sh
python tools/quality.py text
python tools/quality.py format
cmake --preset host
cmake --build --preset host
python tools/quality.py test --build-dir out/host
python tools/quality.py cppcheck --build-dir out/host
python tools/quality.py tidy --build-dir out/host
python tools/quality.py docs
python -m pre_commit run --all-files
```

`text` 和 `format` 可追加文件名；提交钩子传入暂存文件，本地全量检查与 CI 使用同一选择和检查逻辑。格式检查只读，不自动暂存或修改代码。主动格式修复使用固定版本 `clang-format -i`，之后审阅差异。

共享工具版本、默认扫描规则和覆盖率门槛在 xgen-quality 包内维护，`tools/quality.json` 声明本仓采用版本及公开头、生产源码范围。`XGEN_CLANG_FORMAT`、`XGEN_CLANG_TIDY`、`XGEN_CPPCHECK`、`XGEN_DOXYGEN`、`XGEN_GCOVR` 可以指定实际工具路径；指定路径也必须通过版本检查。不要让 PATH 中的 Keil formatter 替代 Nexus 基线所用版本。

检查报告位于 `out/reports`，缺工具、版本不符、规则违规、空的必需测试集、漏跑或跳过测试均返回失败。公开头必须有 Doxygen 文件注释，避免未声明文档文件导致全局 API 被静默忽略；参数等细节继续由严格 Doxygen 验证。工具的 Python unittest 已随实现迁往 xgen-quality，不再在 CRC 维护第二套回归。

## 覆盖率

覆盖率 preset 使用 GCC，需事先准备匹配的 GoogleTest 包与 gcov；与其他编译器的构建目录分开。

```sh
cmake --preset coverage
cmake --build --preset coverage
python tools/quality.py test --build-dir out/coverage
python tools/quality.py coverage --build-dir out/coverage
```

需要时通过 `--gcov-executable` 指定与编译器匹配的 gcov。生产 `.c` 插桩，测试和 GoogleTest 不计入分母；行、函数、分支各至少 80%。`out/reports/coverage.json` 保留行与分支细节，`coverage-summary.json` 提供汇总。重新测量前使用新的覆盖率构建目录或在确认路径归属后清理旧数据，不能用历史运行命中掩盖本次漏测。

## 评审与发布

记录行为基线、RED/GREEN、安装消费、工具版本及实际结果。CI 配置的 Linux/MSVC 矩阵需要由远端执行后才能标记通过；本地 GCC 验证不能替代它。没有板级测量时不声明 MCU 时序或完整 Boot 资源已通过。

启用远端 CI 前，将仓库变量 `XGEN_QUALITY_REPOSITORY` 设置为真实可读取的共享工具仓库 `owner/repository`。CI 使用本仓配置中的固定提交，不跟随工具主分支；地址未设置、提交不可获取或安装版本不匹配都会失败。当前共享工具只有本地仓库，此准备项与远端 CI 均待后续发布任务完成。

本仓库尚未配置远端，也未完成源码授权确认；本地版本号不等同于公开发布。现有 core/link 消费者尚未切换，本次不删除它们的来源实现。
