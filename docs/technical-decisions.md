# MySchematic 技术决策记录

本文档记录了 MySchematic 项目规划和实现过程中的所有关键技术讨论和决策。

---

## 0. Phase 1 实现中的技术决策

### 0.1 AUTOMOC 与头文件列表

**问题**：CMake AUTOMOC 无法找到 `lib/include/myschematic/` 目录下带有 `Q_OBJECT` 宏的头文件，导致 MOC 不生成元对象代码，链接时出现 `undefined reference to vtable` 错误。

**解决方案**：在 `lib/CMakeLists.txt` 的 `add_library()` 中，将所有头文件（`${LIB_HEADERS}`）显式加入目标源文件列表。AUTOMOC 通过扫描目标的源文件来查找 `Q_OBJECT` 宏，因此必须将含有 Q_OBJECT 的头文件纳入目标。

### 0.2 Qt Widgets 可选编译

**问题**：WSL2 环境缺少 OpenGL 头文件（`WrapOpenGL`），导致 `Qt6::Gui` → `Qt6::Widgets` 无法找到。

**解决方案**：将 `BUILD_APP` 设为 CMake 选项（默认 ON），在缺少 Qt Widgets 的环境中可设为 OFF。核心库 `libmyschematic` 仅依赖 `Qt6::Core`，可在任何有 Qt Core 的环境中编译。

### 0.3 测试中用 lambda 替代 QSignalSpy

**问题**：`QSignalSpy` 位于 `Qt6::Test` 模块，该模块需要额外安装且可能依赖 Qt GUI。

**解决方案**：使用 lambda + 计数器模式替代：
```cpp
int count = 0;
QObject::connect(obj, &Class::signal, [&count]() { ++count; });
// ... trigger signal ...
EXPECT_EQ(count, 1);
```
这种方式仅依赖 `Qt6::Core`，与 Google Test 完全兼容。

---

## 1. 技术栈选型

### 讨论背景

需要开发一个面向商业化的模拟电路 Schematic Editor，类似 Cadence Virtuoso，需处理大规模电路（数万到数十万器件），需要高性能和商业可行性。

### 比较的四种技术路线

| 技术路线 | 性能 | 开发效率 | 生态 | 商业可行性 |
|---------|------|---------|------|-----------|
| **Web (React/Canvas)** | 中等，Canvas 性能瓶颈明显 | 快 | 好 | 受限于浏览器 |
| **Python + Qt** | 中低，Python GIL 限制，大规模数据处理慢 | 快 | 好 | 需 cython/C 扩展做性能补丁 |
| **C++ + Qt** | 优秀，直接操作内存，极致性能 | 中等 | EDA 行业标准 | 最佳 |
| **Rust + egui/iced** | 优秀，内存安全 | 较低，GUI 生态不成熟 | 不成熟 | 有风险 |

### 最终决策

**C++ 17 + Qt 6**

理由：
- EDA 行业的事实标准（Cadence、Synopsys、Mentor 均使用 C++）
- 处理 10 万+ 器件的原理图需要极致性能
- Qt 提供了完整的 2D 图形引擎（QGraphicsView/Scene）和成熟的 GUI 组件
- 丰富的第三方库支持
- 商业部署成熟

---

## 2. Qt 许可与版本选择

### 讨论背景

Qt 有多种许可模式，需要确认商业开发是否需要购买商业许可。

### Qt 许可分析

| 许可 | 费用 | 要求 | 适用场景 |
|------|------|------|---------|
| GPL v3 | 免费 | 整个项目必须 GPL | 纯开源项目 |
| **LGPL v3** | 免费 | Qt 库动态链接，修改 Qt 源码需公开 | **我们的选择** |
| Commercial | ~$5000+/年/开发者 | 无开源义务 | 不想公开任何源码 |

### 最终决策

**LGPL v3 + Qt 6 (动态链接)**

关键要点：
- LGPL 允许商业闭源使用，只需动态链接 Qt 库
- Qt 6 中 Schematic 需要的核心模块（Core, Widgets, Gui, OpenGL）全部 LGPL 可用
- 不需要商业许可即可商业发布
- 用户需能替换 Qt 动态库（提供链接说明即可满足 LGPL 要求）

---

## 3. 构建系统

### 最终决策

**CMake**

理由：
- C++ 生态的事实标准
- Qt 6 官方推荐
- 跨平台支持
- 良好的 IDE 集成

---

## 4. 文件格式策略

### 讨论背景

需要同时支持文本格式（可读、git-diff 友好）和二进制格式（大规模电路性能）。

### 行业调研结论

**没有统一的原理图互换标准。** 各工具使用专有格式：
- **Cadence Virtuoso**: OpenAccess 二进制数据库（需 Si2 联盟会员，费用 $5k-$15k/年）
- **KiCad**: S-expression 纯文本（`.kicad_sch`）— 最好的开源参考
- **Altium**: OLE 二进制容器
- **EDIF**: 曾经的交换标准（S-expression），使用率下降
- **SPICE netlist**: 仿真互换标准，但不含图形信息

### 文本格式选择

**JSON** 作为第一阶段文本格式。

对比 KiCad 的 S-expression：
- JSON 生态更广泛，解析库更成熟
- 大多数语言原生支持 JSON
- 可读性相当，git-diff 友好
- S-expression 更紧凑但需要自定义解析

### 二进制格式选择

**延后决定。** 比较了 FlatBuffers vs 自定义二进制：

| 特性 | FlatBuffers | 自定义二进制 |
|------|------------|------------|
| 零拷贝读取 | 支持（核心优势） | 不支持 |
| Schema 演进 | 内建向后兼容 | 需自行实现 |
| 文件大小 | 中等（有对齐填充） | 最小 |
| 控制力 | 受限于 FlatBuffers 约束 | 完全灵活 |
| 实现成本 | 低（codegen） | 高 |

### 序列化抽象接口

为了隔离格式实现：

```cpp
class Serializer {
public:
    virtual bool saveCircuit(const Circuit&, const QString& path) = 0;
    virtual bool loadCircuit(Circuit&, const QString& path) = 0;
    virtual bool saveSymbolLibrary(const SymbolLibrary&, const QString& path) = 0;
    virtual bool loadSymbolLibrary(SymbolLibrary&, const QString& path) = 0;
    virtual QString formatName() const = 0;
    virtual QString fileExtension() const = 0;
};
```

第一阶段实现 `JsonSerializer`，二进制格式延后到性能需要时再决定。

---

## 5. 平台策略

### 最终决策

**Linux 优先，代码兼容 Windows。**

- 主开发和测试在 Linux 上进行
- 使用跨平台的 Qt API，避免平台特定代码
- 平台依赖较大的功能先实现 Linux 版本，Windows 版本后续跟进
- CI 同时在 Linux 和 Windows 上构建测试

---

## 6. Undo/Redo 架构

### 最终决策

**QUndoStack + Command 模式**

- 每个用户操作封装为 `QUndoCommand` 子类
- 支持 `mergeWith()` 合并连续拖动操作
- `CompoundCmd` 用于批量操作（如导入 netlist）
- dirty flag 追踪 `QUndoStack::isClean()`

---

## 7. Library-First 架构

### 讨论背景

核心功能必须编译为静态库，供外部程序集成。GUI 只是库的一个前端。

### 架构设计

**libmyschematic（静态库）** — 只依赖 Qt Core，不依赖 Qt Widgets/GUI

| libmyschematic (无 GUI 依赖) | app (Qt Widgets/Graphics) |
|------------------------------|--------------------------|
| 数据模型 | QGraphicsView/Scene/Item |
| Symbol 定义与库管理 | 绘图工具 |
| 连接性分析 | QUndoCommand 子类 |
| 网表生成与 SPICE 导出 | MainWindow, 菜单, 工具栏 |
| DRC 引擎 | DRC 结果面板 |
| 序列化 | 属性编辑器, Symbol 编辑器 |
| 层次导航逻辑 | 层次导航面板 |

### 集成场景

- Virtuoso 类 EDA 环境嵌入
- AI agent CLI（通过 C++ API 用自然语言控制电路编辑）
- 自动化测试和批处理脚本
- 服务器端 headless 运行

---

## 8. Netlist-to-Schematic (N2S) 自动布局

### 讨论背景

需要从 SPICE netlist（无位置信息）自动生成美观的原理图布局。这是一个核心差异化功能。

### 算法设计

采用**两级布局**策略：

**第一级：功能块级 Sugiyama 布局**
- CircuitAnalyzer 识别常见模拟功能块（差分对、电流镜、共源共栅等）
- 将功能块作为 Sugiyama 图的节点进行层次布局
- 保证信号从左到右流动

**第二级：块内模板布局**
- 差分对：两管水平对称，尾电流源在下
- 电流镜：参考管在左，镜像管在右
- 共源共栅：纵向堆叠
- 反相器：PMOS 在上，NMOS 在下

### Sugiyama 算法四步

1. **去环（Cycle Removal）**：反转反馈边使图变为 DAG
2. **分层（Layer Assignment）**：最长路径法分配层号
3. **交叉最小化（Crossing Minimization）**：重心法（Barycenter Heuristic）
4. **坐标赋值（Coordinate Assignment）**：层号决定 x，层内顺序决定 y

### 布线策略

- 短距离：正交 L 型走线
- 长距离（超过阈值）：使用 Label 代替导线
- 电源连接：使用 PowerSymbol（VDD/GND）
- 避障：Manhattan 路由绕行元件

---

## 9. Hierarchical / Flatten 视图设计

### 讨论背景

需要支持层次化（hierarchical）和展平（flatten）两种显示模式。

### 架构决策

**层次化 Circuit 是唯一的数据源（single source of truth），Flatten 视图是派生产物（只读）。**

```
Circuit (层次化, 唯一数据源)
    ├──→ Hierarchical View (直接渲染, 可编辑)
    └──→ Flatten View (生成式, 只读)
```

### Flatten 为只读的原因

- 一个 SubcircuitDef 可能被实例化多次（X1, X2 都是同一个 diff_pair）
- 在 Flatten 视图编辑 X1 内部的 MOS 管，无法确定映射：只改 X1 实例？还是修改定义（影响所有实例）？
- Virtuoso 的做法也是：flatten view 用于查看/审查，编辑在 hierarchical 模式下 descend 进入子电路完成
- 大幅简化架构，避免双向同步

### 实例聚集约束

展平时保留每个器件的来源实例信息（如 `X1.M1` 来自实例 `X1`），布局算法将同一实例的器件作为一个**布局组（placement group）**：
- 组内器件紧密排列
- 组间保持较大间距
- 可选绘制虚线边框标注实例名
- 布局顺序：先对实例组做 Sugiyama，再在组内用功能块模板

### N2S 模块复用

Phase 5 的 NetlistImporter 在两个场景复用：
1. 从外部 SPICE 文件导入 → 生成可编辑的 schematic
2. 从内部层次化设计展平 → 生成只读的 flatten 查看视图

---

## 10. OpenAccess 评估

### 调研结论

OpenAccess 是 Si2 (Silicon Integration Initiative) 联盟维护的 EDA 数据库标准：
- 需要 Si2 联盟会员资格
- 会员费约 $5,000 - $15,000/年（取决于公司规模）
- 主要用于 IC 设计工具之间的数据互操作

### 决策

**当前阶段不使用 OpenAccess。** 原因：
- 会员费用门槛
- 我们的自定义格式（JSON + 未来二进制）已足够
- SPICE netlist 作为标准互换格式已覆盖主要需求
- 后续如有需要，可作为可选的 interop 层添加

---

## 11. 测试策略

### 框架选择

**Google Test + Qt Test**

- Google Test 用于纯逻辑单元测试
- Qt Test 用于需要 QObject/信号槽的测试
- 每个 Phase 都有对应的测试用例集

### 测试分层

1. **单元测试**：各模块独立测试（model, symbol, netlist, serialization, drc）
2. **集成测试**：模块间协作测试（端到端 SPICE 导入、序列化往返）
3. **API 测试**：独立程序链接 libmyschematic.a 验证公开 API
4. **性能测试**：10k/50k/100k 元件基准测试

### 测试数据

预定义的测试电路文件：
- `inverter.msch.json` — CMOS 反相器
- `diff_pair.msch.json` — 差分对
- `opamp_twostage.msch.json` — 两级运放
- `bandgap.msch.json` — 带隙基准
- `hierarchical_opamp.msch.json` — 层次化运放
- `large_10k.msch.json` — 10000 元件性能测试

SPICE netlist 测试文件：
- `inverter.sp`, `diff_pair.sp`, `twostage_opamp.sp`, `bandgap.sp`, `folded_cascode.sp`
