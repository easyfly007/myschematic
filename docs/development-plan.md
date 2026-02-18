# MySchematic — 模拟电路 Schematic Editor 开发计划

## 开发进度

| Phase | 名称 | 状态 | 完成日期 | 说明 |
|-------|------|------|----------|------|
| **Phase 1** | 基础框架与核心数据模型 | **已完成** | 2026-02-18 | lib 编译通过，37 个测试全部通过，app 代码已写好（需 Qt Widgets 环境） |
| **Phase 2** | Symbol 系统与基础元件 | **代码已写完，待验证** | — | 所有头文件/源文件/测试/app 代码已创建，lib 单独编译通过，需要做完整 build+test 验证 |
| Phase 3 | 导线连接与连接性分析 | 未开始 | — | |
| Phase 4 | 序列化与文件管理 | 未开始 | — | |
| Phase 5 | Netlist 导入与自动布局 | 未开始 | — | |
| Phase 6 | 层次化设计 | 未开始 | — | |
| Phase 7 | Symbol 编辑器 | 未开始 | — | |
| Phase 8 | DRC 检查 | 未开始 | — | |
| Phase 9 | 完善与优化 | 未开始 | — | |

### Phase 1 完成详情

**已实现的文件（46 个）**：

| 模块 | 文件数 | 说明 |
|------|--------|------|
| lib/include/myschematic/ | 8 | 公开 API 头文件：types.h, geometry.h, schematic_element.h, component.h, sheet.h, circuit.h, id_generator.h, myschematic.h |
| lib/src/model/ | 5 | 核心模型实现：id_generator.cpp, schematic_element.cpp, component.cpp, sheet.cpp, circuit.cpp |
| app/ | 23 | MainWindow, SchematicScene/View, ComponentItem, SelectTool, PlaceTool, 3 个 UndoCommand (Add/Delete/Move) |
| tests/ | 5 | test_circuit (6 tests), test_sheet (9 tests), test_component (12 tests), test_move_cmd (4 tests), test_add_delete_cmd (6 tests) |
| cmake/ | 1 | CompilerWarnings.cmake |
| 根目录 | 2 | CMakeLists.txt, .gitignore |

**构建产物**：
- `libmyschematic.a` — 核心静态库，编译通过
- 5 个测试可执行文件 — 全部通过（37 个 test case，0.13s）
- GUI 应用代码已写好 — 需要完整 Qt Widgets 环境（含 OpenGL 头文件）才能编译

**环境适配说明**：
- 开发环境为 WSL2，缺少 OpenGL 头文件，因此 GUI 应用（`BUILD_APP`）设为 OFF
- Qt6 通过 `aqtinstall` 安装到 `~/Qt/6.5.3/gcc_64`
- Google Test 从源码编译安装到 `~/gtest-install`
- 测试使用 lambda 信号计数器替代 QSignalSpy（后者需要 Qt Test 模块）

---

## Context

开发一个面向商业化的模拟电路 Schematic Editor，对标 Cadence Virtuoso Schematic Editor。需要处理大规模电路（数万到数十万器件），支持层次化设计、Symbol 编辑、DRC 检查、SPICE netlist 导出。

**关键架构约束**：核心功能必须编译为**静态库 (libmyschematic)**，提供 C++ API 供外部程序集成。GUI 应用只是库的一个前端。宿主程序可以是 Virtuoso 类环境、AI agent CLI 等。

## 技术决策

| 决策 | 结论 |
|------|------|
| 技术栈 | C++ 17 + Qt 6 (LGPL, 动态链接) |
| 构建系统 | CMake |
| 文件格式 | 序列化接口抽象；第一阶段实现 JSON；二进制格式延后 |
| 平台 | Linux 优先，代码兼容 Windows |
| Undo/Redo | QUndoStack + Command 模式 |
| 测试框架 | Google Test + Qt Test |

## 关于文件格式的行业调研结论

**没有统一的原理图互换标准。** 各工具使用专有格式：
- **Cadence Virtuoso**: OpenAccess 二进制数据库
- **KiCad**: S-expression 纯文本（`.kicad_sch`）— 最好的开源参考
- **Altium**: OLE 二进制容器
- **EDIF**: 曾经的交换标准（S-expression），现在使用率下降
- **SPICE netlist**: 仿真互换标准，但不含图形信息

**我们的策略**：自定义格式 + 标准互换导出（SPICE/CDL）。文本模式参考 KiCad 的设计哲学（可读、git-diff 友好），但使用 JSON 作为第一阶段实现（生态更广泛、解析库更成熟）。

---

## 一、项目架构

### 核心原则：Library-First

```
myschematic/
├── CMakeLists.txt
├── cmake/
│   ├── CompilerWarnings.cmake
│   └── QtHelpers.cmake
│
├── lib/                          ← 核心静态库 (libmyschematic)
│   ├── CMakeLists.txt            ← 生成 libmyschematic.a
│   ├── include/myschematic/      ← 公开头文件 (API)
│   │   ├── myschematic.h         ← 总头文件
│   │   ├── circuit.h
│   │   ├── sheet.h
│   │   ├── component.h
│   │   ├── wire.h
│   │   ├── pin.h
│   │   ├── net.h
│   │   ├── port.h
│   │   ├── label.h
│   │   ├── property.h
│   │   ├── symbol_def.h
│   │   ├── symbol_library.h
│   │   ├── library_manager.h
│   │   ├── connection_graph.h
│   │   ├── netlist.h
│   │   ├── spice_exporter.h
│   │   ├── serializer.h
│   │   ├── drc_engine.h
│   │   ├── hierarchy_navigator.h
│   │   ├── spice_parser.h
│   │   ├── netlist_importer.h     ← Netlist→Schematic 自动生成入口
│   │   ├── geometry.h
│   │   └── types.h               ← 公共枚举和类型定义
│   └── src/                      ← 实现文件 (不暴露)
│       ├── model/
│       │   ├── circuit.cpp
│       │   ├── sheet.cpp
│       │   ├── component.cpp
│       │   ├── wire.cpp
│       │   ├── pin.cpp
│       │   ├── net.cpp
│       │   ├── port.cpp
│       │   ├── label.cpp
│       │   ├── property.cpp
│       │   └── id_generator.cpp
│       ├── symbol/
│       │   ├── symbol_def.cpp
│       │   ├── symbol_pin.cpp
│       │   ├── symbol_graphic.cpp
│       │   ├── symbol_library.cpp
│       │   ├── library_manager.cpp
│       │   └── builtin_symbols.cpp
│       ├── netlist/
│       │   ├── connection_graph.cpp
│       │   ├── net_resolver.cpp
│       │   ├── netlist.cpp
│       │   └── hierarchy_traverser.cpp
│       ├── io/
│       │   ├── serializer.cpp
│       │   ├── json_serializer.cpp
│       │   ├── file_manager.cpp
│       │   ├── spice_exporter.cpp
│       │   └── spice_parser.cpp
│       ├── import/                    ← Netlist → Schematic 自动布局
│       │   ├── netlist_importer.cpp   ← 入口：整合解析+布局+布线
│       │   ├── circuit_analyzer.cpp   ← 电路拓扑分析，识别功能块
│       │   ├── schematic_placer.cpp   ← 自动放置算法
│       │   └── schematic_router.cpp   ← 自动布线算法
│       ├── drc/
│       │   ├── drc_engine.cpp
│       │   ├── drc_violation.cpp
│       │   └── rules/
│       │       ├── floating_pin_rule.cpp
│       │       ├── short_circuit_rule.cpp
│       │       ├── unconnected_net_rule.cpp
│       │       ├── duplicate_ref_rule.cpp
│       │       └── missing_property_rule.cpp
│       └── hierarchy/
│           ├── subcircuit_def.cpp
│           ├── hierarchy_model.cpp
│           ├── hierarchy_navigator.cpp
│           └── sheet_instance.cpp
│
├── app/                          ← GUI 应用 (依赖 libmyschematic + Qt Widgets)
│   ├── CMakeLists.txt            ← 生成 myschematic 可执行文件
│   ├── main.cpp
│   ├── mainwindow.h / .cpp
│   ├── action_manager.h / .cpp
│   ├── preferences.h / .cpp
│   ├── view/
│   │   ├── schematic_scene.h / .cpp
│   │   ├── schematic_view.h / .cpp
│   │   ├── items/
│   │   │   ├── component_item.h / .cpp
│   │   │   ├── wire_item.h / .cpp
│   │   │   ├── pin_item.h / .cpp
│   │   │   ├── label_item.h / .cpp
│   │   │   ├── port_item.h / .cpp
│   │   │   ├── junction_item.h / .cpp
│   │   │   └── power_symbol_item.h / .cpp
│   │   ├── rendering/
│   │   │   ├── color_scheme.h / .cpp
│   │   │   └── render_settings.h / .cpp
│   │   └── overlay/
│   │       ├── grid_overlay.h / .cpp
│   │       └── ruler_overlay.h / .cpp
│   ├── tools/
│   │   ├── tool.h / .cpp
│   │   ├── tool_manager.h / .cpp
│   │   ├── select_tool.h / .cpp
│   │   ├── wire_tool.h / .cpp
│   │   ├── place_tool.h / .cpp
│   │   ├── label_tool.h / .cpp
│   │   └── zoom_tool.h / .cpp
│   ├── commands/
│   │   ├── add_component_cmd.h / .cpp
│   │   ├── delete_element_cmd.h / .cpp
│   │   ├── move_element_cmd.h / .cpp
│   │   ├── add_wire_cmd.h / .cpp
│   │   ├── rotate_element_cmd.h / .cpp
│   │   ├── mirror_element_cmd.h / .cpp
│   │   ├── change_property_cmd.h / .cpp
│   │   ├── copy_paste_cmd.h / .cpp
│   │   └── compound_cmd.h / .cpp
│   ├── editors/
│   │   ├── symbol_editor.h / .cpp
│   │   ├── symbol_editor_scene.h / .cpp
│   │   ├── property_editor.h / .cpp
│   │   └── library_browser.h / .cpp
│   └── panels/
│       ├── hierarchy_panel.h / .cpp
│       ├── drc_results_panel.h / .cpp
│       └── component_palette.h / .cpp
│
├── tests/                        ← 测试
│   ├── CMakeLists.txt
│   ├── testdata/                 ← 测试用的 schematic 文件
│   │   ├── inverter.msch.json
│   │   ├── diff_pair.msch.json
│   │   ├── opamp_twostage.msch.json
│   │   ├── bandgap.msch.json
│   │   ├── hierarchical_opamp.msch.json
│   │   ├── large_10k.msch.json
│   │   └── symbols/
│   │       └── basic_analog.mschlib.json
│   ├── test_model/
│   ├── test_symbol/
│   ├── test_serialization/
│   ├── test_netlist/
│   ├── test_spice_export/
│   ├── test_drc/
│   ├── test_hierarchy/
│   ├── test_import/              ← Netlist 导入 + 自动布局测试
│   │   ├── spice_inputs/         ← 测试用 SPICE netlist 文件
│   │   │   ├── inverter.sp
│   │   │   ├── diff_pair.sp
│   │   │   ├── twostage_opamp.sp
│   │   │   ├── bandgap.sp
│   │   │   └── folded_cascode.sp
│   └── test_api/                 ← 库 API 集成测试
│
└── resources/
    ├── icons/
    ├── symbols/                  ← 内置符号库
    └── themes/
```

### lib 与 app 的分界线

| libmyschematic (无 GUI 依赖) | app (Qt Widgets/Graphics) |
|------------------------------|--------------------------|
| 数据模型 (Circuit, Sheet, Component, Wire...) | QGraphicsView/Scene/Item |
| Symbol 定义与库管理 | 绘图工具 (SelectTool, WireTool...) |
| 连接性分析 (ConnectionGraph) | QUndoCommand 子类 |
| 网表生成与 SPICE 导出 | MainWindow, 菜单, 工具栏 |
| DRC 引擎 | DRC 结果面板 |
| 序列化 (JSON/Binary) | 属性编辑器, Symbol 编辑器 |
| 层次导航逻辑 | 层次导航面板 |

**libmyschematic 只依赖 Qt Core**（QString, QList, QJsonDocument 等），不依赖 Qt Widgets/GUI。这确保它可以在无头环境（headless）中使用——比如 AI agent CLI。

---

## 二、Schematic 文件格式规范 (v1.0)

### 2.1 文件扩展名

| 扩展名 | 用途 |
|--------|------|
| `.msch.json` | 原理图文本格式 |
| `.msch` | 原理图二进制格式 (Phase 2+) |
| `.mschlib.json` | 符号库文本格式 |
| `.mschlib` | 符号库二进制格式 (Phase 2+) |

### 2.2 JSON Schema 定义

```json
{
  "formatVersion": "1.0",
  "generator": "myschematic-1.0.0",
  "createdAt": "2026-02-18T12:00:00Z",
  "modifiedAt": "2026-02-18T12:30:00Z",

  "metadata": {
    "title": "Two-Stage Op-Amp",
    "author": "Designer Name",
    "description": "A two-stage CMOS operational amplifier",
    "revision": "1.0"
  },

  "libraryRefs": [
    { "name": "basic_analog", "path": "symbols/basic_analog.mschlib.json" }
  ],

  "sheets": [
    {
      "id": "uuid-sheet-1",
      "name": "top",
      "gridSize": 10.0,

      "components": [
        {
          "id": "uuid-comp-1",
          "symbolRef": { "library": "basic_analog", "symbol": "nmos4" },
          "instanceName": "M1",
          "position": { "x": 200.0, "y": 300.0 },
          "rotation": 0,
          "mirror": false,
          "properties": {
            "W": "10u",
            "L": "1u",
            "nf": "1",
            "model": "nch"
          }
        }
      ],

      "wires": [
        {
          "id": "uuid-wire-1",
          "points": [
            { "x": 230.0, "y": 300.0 },
            { "x": 300.0, "y": 300.0 },
            { "x": 300.0, "y": 200.0 }
          ]
        }
      ],

      "labels": [
        {
          "id": "uuid-label-1",
          "name": "VIN",
          "position": { "x": 150.0, "y": 300.0 },
          "orientation": "horizontal"
        }
      ],

      "ports": [
        {
          "id": "uuid-port-1",
          "name": "VOUT",
          "direction": "output",
          "position": { "x": 500.0, "y": 300.0 }
        }
      ],

      "powerSymbols": [
        {
          "id": "uuid-pwr-1",
          "type": "gnd",
          "position": { "x": 200.0, "y": 400.0 }
        },
        {
          "id": "uuid-pwr-2",
          "type": "vdd",
          "netName": "VDD",
          "position": { "x": 200.0, "y": 100.0 }
        }
      ],

      "junctions": [
        {
          "id": "uuid-junc-1",
          "position": { "x": 300.0, "y": 300.0 }
        }
      ],

      "subcircuitInstances": [
        {
          "id": "uuid-sub-1",
          "subcircuitRef": "uuid-subckt-def-1",
          "instanceName": "X1",
          "position": { "x": 400.0, "y": 200.0 },
          "rotation": 0,
          "mirror": false,
          "parameters": {
            "W": "20u"
          }
        }
      ]
    }
  ],

  "subcircuitDefs": [
    {
      "id": "uuid-subckt-def-1",
      "name": "diff_pair",
      "ports": [
        { "name": "INP", "direction": "input" },
        { "name": "INM", "direction": "input" },
        { "name": "OUTP", "direction": "output" },
        { "name": "OUTM", "direction": "output" },
        { "name": "VDD", "direction": "inout" },
        { "name": "VSS", "direction": "inout" }
      ],
      "parameters": [
        { "name": "W", "defaultValue": "10u" },
        { "name": "L", "defaultValue": "1u" }
      ],
      "symbol": {
        "graphics": [
          { "type": "rectangle", "x": -40, "y": -30, "width": 80, "height": 60 }
        ],
        "pins": [
          { "name": "INP", "x": -40, "y": -10, "direction": "left" },
          { "name": "INM", "x": -40, "y": 10, "direction": "left" },
          { "name": "OUTP", "x": 40, "y": -10, "direction": "right" },
          { "name": "OUTM", "x": 40, "y": 10, "direction": "right" },
          { "name": "VDD", "x": 0, "y": -30, "direction": "up" },
          { "name": "VSS", "x": 0, "y": 30, "direction": "down" }
        ]
      },
      "sheet": { "... 内嵌 sheet 结构，同上 ..." }
    }
  ]
}
```

### 2.3 Symbol Library 文件格式

```json
{
  "formatVersion": "1.0",
  "libraryName": "basic_analog",

  "symbols": [
    {
      "name": "nmos4",
      "description": "N-Channel MOSFET (4-terminal)",
      "category": "MOSFET",
      "pins": [
        { "name": "G", "number": 1, "x": -30, "y": 0,   "direction": "left",  "electricalType": "input" },
        { "name": "D", "number": 2, "x": 0,   "y": -20, "direction": "up",    "electricalType": "passive" },
        { "name": "S", "number": 3, "x": 0,   "y": 20,  "direction": "down",  "electricalType": "passive" },
        { "name": "B", "number": 4, "x": 10,  "y": 0,   "direction": "right", "electricalType": "passive" }
      ],
      "graphics": [
        { "type": "line", "x1": -30, "y1": 0, "x2": -10, "y2": 0 },
        { "type": "line", "x1": -10, "y1": -15, "x2": -10, "y2": 15 },
        { "type": "line", "x1": -5, "y1": -15, "x2": -5, "y2": -5 },
        { "type": "line", "x1": -5, "y1": -10, "x2": 0, "y2": -10 },
        { "type": "line", "x1": 0, "y1": -20, "x2": 0, "y2": -10 },
        { "type": "line", "x1": -5, "y1": 5, "x2": -5, "y2": 15 },
        { "type": "line", "x1": -5, "y1": 10, "x2": 0, "y2": 10 },
        { "type": "line", "x1": 0, "y1": 10, "x2": 0, "y2": 20 },
        { "type": "line", "x1": -5, "y1": 0, "x2": 0, "y2": 0 },
        { "type": "line", "x1": 0, "y1": 0, "x2": 10, "y2": 0 },
        { "type": "polyline", "points": [{"x":-5,"y":-2},{"x":-2,"y":0},{"x":-5,"y":2}], "filled": true }
      ],
      "propertyTemplates": [
        { "name": "model", "defaultValue": "nch", "required": true, "visible": true },
        { "name": "W", "defaultValue": "1u", "required": true, "visible": true },
        { "name": "L", "defaultValue": "100n", "required": true, "visible": true },
        { "name": "nf", "defaultValue": "1", "required": false, "visible": false },
        { "name": "m", "defaultValue": "1", "required": false, "visible": false }
      ],
      "spiceTemplate": "M{instanceName} {D} {G} {S} {B} {model} W={W} L={L} nf={nf} m={m}"
    }
  ]
}
```

### 2.4 设计原则

- **UUID 标识所有对象**：避免重命名导致的 diff 噪音
- **坐标值整数化**：网格对齐后坐标为整数，避免浮点精度问题
- **确定性排序**：序列化时按 UUID 排序，减少无意义 diff
- **向后兼容**：反序列化忽略未知字段，缺失字段使用默认值
- **文本/二进制无损互转**：两种格式表达完全相同的数据模型

---

## 三、核心 API 设计 (libmyschematic)

### 3.1 公开 API 概览

```cpp
// === 顶层容器 ===
namespace myschematic {

class Circuit {
public:
    Circuit();
    ~Circuit();

    // Sheet 管理
    Sheet* createSheet(const QString& name);
    void removeSheet(const QString& id);
    Sheet* sheetById(const QString& id) const;
    Sheet* sheetByName(const QString& name) const;
    QList<Sheet*> sheets() const;

    // Subcircuit 管理
    SubcircuitDef* createSubcircuit(const QString& name);
    void removeSubcircuit(const QString& id);
    SubcircuitDef* subcircuitById(const QString& id) const;
    QList<SubcircuitDef*> subcircuits() const;

    // 库管理
    LibraryManager* libraryManager() const;

    // 序列化
    bool save(const QString& path, Serializer* serializer = nullptr);
    bool load(const QString& path, Serializer* serializer = nullptr);

    // 全局操作
    Netlist generateNetlist(HierarchyTraverser::Mode mode) const;
    QList<DrcViolation> runDrc(const DrcEngine& engine) const;
};

// === Sheet 操作 ===
class Sheet {
public:
    QString id() const;
    QString name() const;

    // 元件操作
    Component* addComponent(const SymbolDef* symbolDef, const QPointF& pos);
    void removeElement(const QString& id);
    Component* componentById(const QString& id) const;
    QList<Component*> components() const;

    // 导线操作
    Wire* addWire(const QList<QPointF>& points);
    QList<Wire*> wires() const;

    // 标号
    Label* addLabel(const QString& name, const QPointF& pos);
    QList<Label*> labels() const;

    // 端口 (层次设计用)
    Port* addPort(const QString& name, PortDirection dir, const QPointF& pos);
    QList<Port*> ports() const;

    // 查询
    QList<SchematicElement*> elementsAt(const QPointF& pos, qreal radius = 5.0) const;
    QList<SchematicElement*> elementsInRect(const QRectF& rect) const;

    // 连接性
    ConnectionGraph* connectionGraph() const;  // 惰性计算，修改后自动失效
};

// === SPICE 导出 ===
class SpiceExporter {
public:
    enum class Dialect { Generic, HSpice, Spectre, LTSpice };

    void setDialect(Dialect d);
    void setTitle(const QString& title);
    QString exportToString(const Netlist& netlist) const;
    bool exportToFile(const Netlist& netlist, const QString& path) const;
};

// === DRC ===
class DrcEngine {
public:
    void addRule(std::unique_ptr<DrcRule> rule);
    void enableRule(const QString& ruleName, bool enabled);
    QList<DrcViolation> run(const Sheet* sheet, const ConnectionGraph* graph) const;
    QList<DrcViolation> runAll(const Circuit& circuit) const;  // 全层次
};

// === SPICE 解析 ===
class SpiceParser {
public:
    struct ParseResult {
        QString title;
        QList<NetlistEntry> entries;
        QList<SubcircuitNetlist> subcircuits;  // .SUBCKT 定义
        QStringList includes;
        QStringList errors;
    };

    ParseResult parse(const QString& spiceText);
    ParseResult parseFile(const QString& path);
};

// === Netlist → Schematic 自动生成 ===
class NetlistImporter {
public:
    struct ImportOptions {
        qreal gridSize = 10.0;
        qreal minComponentSpacing = 80.0;
        qreal layerSpacing = 200.0;          // 级间水平距离
        bool useLabelsForLongNets = true;     // 长距离连接用标号代替走线
        int longNetThreshold = 500;           // 距离阈值
        bool recognizePatterns = true;        // 是否识别功能块(差分对等)
        bool addPowerSymbols = true;          // VDD/GND 用 power symbol
    };

    // 从 SPICE netlist 文本生成完整 Sheet（含位置和连线）
    Sheet* importFromSpice(const QString& spiceText,
                           LibraryManager* libMgr,
                           const ImportOptions& opts = {});

    // 从 SPICE 文件导入
    Sheet* importFromSpiceFile(const QString& path,
                               LibraryManager* libMgr,
                               const ImportOptions& opts = {});

    QStringList warnings() const;  // 无法识别的器件类型等
};

} // namespace myschematic
```

### 3.2 AI Agent / CLI 集成示例

```cpp
#include <myschematic/myschematic.h>

int main() {
    myschematic::Circuit circuit;
    circuit.load("amplifier.msch.json");

    auto* sheet = circuit.sheetByName("top");

    // AI agent 通过 API 操作电路
    auto* nmos = sheet->addComponent(
        circuit.libraryManager()->findSymbol("basic_analog", "nmos4"),
        QPointF(200, 300));
    nmos->setProperty("W", "10u");
    nmos->setProperty("L", "100n");
    nmos->setInstanceName("M1");

    // 生成网表
    auto netlist = circuit.generateNetlist(HierarchyTraverser::Mode::Flatten);

    // 导出 SPICE
    myschematic::SpiceExporter exporter;
    exporter.setDialect(SpiceExporter::Dialect::HSpice);
    exporter.exportToFile(netlist, "amplifier.sp");

    // 运行 DRC
    myschematic::DrcEngine drc;
    drc.addRule(std::make_unique<myschematic::FloatingPinRule>());
    auto violations = drc.runAll(circuit);
    for (const auto& v : violations) {
        qDebug() << v.ruleName() << v.message();
    }

    circuit.save("amplifier.msch.json");
}
```

---

## 四、分阶段开发计划

### Phase 1: 基础框架与核心数据模型 — **已完成**

**目标**：搭建 CMake 项目结构，实现核心数据模型 (lib)，实现最小可运行的 GUI 应用。画布上能放置占位方块、选择、移动，所有操作支持 Undo/Redo。

**可交付物**：能启动的 Qt 应用 + 可编译的 libmyschematic.a（含基础数据模型）

#### 实现规格

**lib/ 部分**：
- `types.h` — 公共枚举：`ElementType { Component, Wire, Label, Port, PowerSymbol, Junction }`、`PortDirection { Input, Output, InOut }`
- `geometry.h` — `Point`（封装 QPointF）、`Rect`（封装 QRectF）。坐标使用 `double`，序列化时取整到网格
- `SchematicElement` — 抽象基类：`id() -> QString (UUID)`、`position()`、`setPosition()`、`rotation()`、`setRotation()`、`isMirrored()`、`setMirrored()`、`boundingRect()`、`type()`。继承 QObject，发射 `changed()` 信号
- `Sheet` — 持有 `QList<SchematicElement*>`，提供增删查接口，发射 `elementAdded/Removed/Changed` 信号
- `Circuit` — 持有 `QList<Sheet*>`，提供 Sheet 管理接口
- `Component` — 继承 SchematicElement，此阶段只有 position + boundingRect（固定 60x40 占位矩形）
- `IdGenerator` — 基于 `QUuid::createUuid()` 生成 UUID 字符串

**app/ 部分**：
- `MainWindow` — QMainWindow，中央为 SchematicView，菜单栏（File/Edit/View/Tools），工具栏（选择/放置/Undo/Redo），状态栏
- `SchematicView` — QGraphicsView 子类：鼠标滚轮缩放（以鼠标位置为中心）、中键拖动平移、Ctrl+0 适合窗口、最小/最大缩放限制
- `SchematicScene` — QGraphicsScene 子类：监听 Sheet 信号，同步创建/销毁 QGraphicsItem
- `ComponentItem` — QGraphicsItem 子类：绘制蓝色矩形 + 实例名文字，选中时显示虚线边框 + 8 个 handle
- `GridOverlay` — 在 `SchematicView::drawBackground()` 中绘制网格点阵，网格间距 10px
- `SelectTool` — 点击选中（单选）、Shift+点击（多选）、空白处拖拽（框选）、选中后拖拽（移动）
- `PlaceTool` — 点击放置 Component，ESC 退出
- `ToolManager` — 管理当前 Tool，分发鼠标/键盘事件
- `AddComponentCmd` / `DeleteElementCmd` / `MoveElementCmd` — QUndoCommand 子类
- `MoveElementCmd` 支持 `mergeWith()` — 连续拖动合并为一个 undo 步骤

#### 测试用例 (Phase 1)

```
test_model/
  test_circuit.cpp:
    - TEST(Circuit, CreateSheet): 创建 Sheet，验证 id 非空、name 正确
    - TEST(Circuit, RemoveSheet): 删除 Sheet，验证列表为空
    - TEST(Circuit, DuplicateSheetName): 同名 Sheet 应返回 nullptr 或报错
  test_sheet.cpp:
    - TEST(Sheet, AddComponent): 添加 Component，验证 elements() 包含它
    - TEST(Sheet, RemoveElement): 删除后 elementById 返回 nullptr
    - TEST(Sheet, ElementsInRect): 验证空间查询返回正确元素
    - TEST(Sheet, SignalEmission): 验证 elementAdded 信号被发射
  test_component.cpp:
    - TEST(Component, DefaultState): 新建 Component，位置 (0,0)，旋转 0，未镜像
    - TEST(Component, SetPosition): 设置位置后 position() 返回新值
    - TEST(Component, BoundingRect): 验证包围盒尺寸正确
    - TEST(Component, Rotation): 旋转 90 度后 boundingRect 交换宽高

test_commands/
  test_move_cmd.cpp:
    - TEST(MoveCmd, Redo): push 后 Component 在新位置
    - TEST(MoveCmd, Undo): undo 后 Component 回到原位置
    - TEST(MoveCmd, Merge): 两个连续 MoveCmd 合并为一个
  test_add_delete_cmd.cpp:
    - TEST(AddCmd, RedoUndo): redo 添加，undo 移除
    - TEST(DeleteCmd, RedoUndo): redo 删除，undo 恢复
```

---

### Phase 2: Symbol 系统与基础元件

**目标**：实现 SymbolDef/实例架构，内置 CMOS 基本元件，元件以真实电路符号显示，支持旋转/镜像/属性编辑。

**可交付物**：能放置 NMOS/PMOS/R/C/L/电源 等真实元件符号，有属性编辑面板。

#### 实现规格

**lib/ 部分**：
- `SymbolPin` — name, pinNumber, offset(QPointF), direction(Left/Right/Up/Down), electricalType(Input/Output/Passive/Power/Bidirectional)
- `SymbolGraphic` — 多态基类，子类型：`GraphicLine(x1,y1,x2,y2)`, `GraphicRect(x,y,w,h,filled)`, `GraphicCircle(cx,cy,r,filled)`, `GraphicArc(cx,cy,r,startAngle,spanAngle)`, `GraphicPolyline(points,filled)`, `GraphicText(x,y,text,fontSize,anchor)`
- `SymbolDef` — name, libraryName, pins[], graphics[], propertyTemplates[], spiceTemplate, boundingRect()（由 graphics 计算）
- `SymbolLibrary` — name, symbols[] (QMap<QString, SymbolDef*>), findSymbol(name)
- `LibraryManager` — libraries[], addLibrary(), findSymbol(libName, symName), searchPaths
- `BuiltinSymbols` — 静态工厂方法创建所有内置元件
- `Property` — struct: name, value, defaultValue, required, visible
- `PropertyBag` — QMap<QString, Property>, get/set/has/remove
- `Component` 扩展 — 增加 `symbolDef()`、`pins()`、`properties()`、`instanceName()`、`pinWorldPosition(pinName)`
- `Pin` — 引用 SymbolPin，属于 Component 实例，`worldPosition()` 由 Component.position + SymbolPin.offset + 旋转/镜像变换计算

#### 内置元件清单与引脚定义

| 元件 | Symbol Name | 引脚 | SPICE 模板 |
|------|-------------|------|-----------|
| NMOS | nmos4 | G, D, S, B | `M{inst} {D} {G} {S} {B} {model} W={W} L={L}` |
| PMOS | pmos4 | G, D, S, B | `M{inst} {D} {G} {S} {B} {model} W={W} L={L}` |
| Resistor | resistor | P, N | `R{inst} {P} {N} {value}` |
| Capacitor | capacitor | P, N | `C{inst} {P} {N} {value}` |
| Inductor | inductor | P, N | `L{inst} {P} {N} {value}` |
| Diode | diode | A, K | `D{inst} {A} {K} {model}` |
| NPN BJT | npn | B, C, E | `Q{inst} {C} {B} {E} {model}` |
| PNP BJT | pnp | B, C, E | `Q{inst} {C} {B} {E} {model}` |
| VSource | vsource | P, N | `V{inst} {P} {N} {value}` |
| ISource | isource | P, N | `I{inst} {P} {N} {value}` |
| VCVS | vcvs | NP,NN,CP,CN | `E{inst} {NP} {NN} {CP} {CN} {gain}` |
| VCCS | vccs | NP,NN,CP,CN | `G{inst} {NP} {NN} {CP} {CN} {gm}` |
| CCVS | ccvs | NP,NN,CP,CN | `H{inst} {NP} {NN} {CP} {CN} {rm}` |
| CCCS | cccs | NP,NN,CP,CN | `F{inst} {NP} {NN} {CP} {CN} {gain}` |
| IdealOpAmp | opamp | INP,INM,OUT | (behavioral model) |
| GND | gnd | PIN | (power net) |
| VDD | vdd | PIN | (power net) |

#### 测试用例 (Phase 2)

```
test_symbol/
  test_symbol_def.cpp:
    - TEST(SymbolDef, BoundingRect): 验证根据 graphics 计算的包围盒
    - TEST(SymbolDef, PinByName): 查找引脚，不存在返回 nullptr
    - TEST(SymbolDef, PropertyTemplates): 验证默认属性
  test_builtin_symbols.cpp:
    - TEST(Builtin, Nmos4Pins): 验证 NMOS 有 G/D/S/B 四个引脚
    - TEST(Builtin, Nmos4Graphics): 验证图形原语数量 > 0
    - TEST(Builtin, AllSymbolsValid): 遍历所有内置 symbol，验证非空、引脚数 > 0
  test_component_extended.cpp:
    - TEST(Component, PinWorldPosition_NoRotation): 验证引脚世界坐标
    - TEST(Component, PinWorldPosition_Rotated90): 旋转 90 度后引脚坐标正确变换
    - TEST(Component, PinWorldPosition_Mirrored): 镜像后引脚坐标正确变换
    - TEST(Component, Properties): 设置/获取属性值
  test_library.cpp:
    - TEST(Library, AddAndFind): 添加 symbol 后能按名字找到
    - TEST(LibraryManager, FindAcrossLibraries): 跨库查找
```

---

### Phase 3: 导线连接与连接性分析

**目标**：实现导线绘制、引脚连接、网络标号、连接性图构建、基础 SPICE netlist 导出。

**可交付物**：能在元件间画导线、自动识别连接、导出可用的 SPICE netlist。

#### 实现规格

**lib/ 部分**：
- `Wire` — 继承 SchematicElement，`points() -> QList<QPointF>`，至少 2 个点
- `Label` — 继承 SchematicElement，`name()`、`orientation()`
- `PowerSymbol` — 继承 SchematicElement，`powerType() (GND/VDD/Custom)`、`netName()`
- `Junction` — 继承 SchematicElement，标记三线交汇（位置即为数据）
- `Net` — `id()`、`name()`、`pins() -> QList<Pin*>`、`wires() -> QList<Wire*>`、`labels() -> QList<Label*>`
- `ConnectionGraph` — 核心连接性引擎：
  - `build(const Sheet*)` — 全量构建：收集所有端点，基于坐标重合使用 Union-Find 算法合并为 Net
  - 内部使用空间哈希加速端点匹配
- `Netlist` — `entries() -> QList<NetlistEntry>`，`nets() -> QList<Net>`
- `SpiceExporter` — 遍历 Netlist 生成 SPICE 文本

**app/ 部分**：
- `WireTool` — 正交导线绘制状态机
- `WireItem` / `LabelItem` / `JunctionItem` / `PowerSymbolItem`
- 自动 Junction：当新导线与已有导线交叉且共享端点时，自动添加 Junction

#### 测试用例 (Phase 3)

```
test_netlist/
  test_connection_graph.cpp:
    - TEST(ConnGraph, TwoComponentsOneWire): 两个电阻串联，验证 3 个 net
    - TEST(ConnGraph, LabelMergesNets): 两段不相连的 wire 有相同 label，应合并为 1 个 net
    - TEST(ConnGraph, PowerNetGlobal): GND 符号隐式创建全局 "0" net
    - TEST(ConnGraph, Junction): 三线交汇有 junction 时合并为 1 个 net
    - TEST(ConnGraph, NoJunctionNoCross): 两线十字交叉无 junction，各自独立
    - TEST(ConnGraph, FloatingWire): 未连接到引脚的导线独立成 net

  test_netlist.cpp:
    - TEST(Netlist, InverterCircuit): CMOS inverter 验证 2 个器件，4 个 net

test_spice_export/
  test_spice_exporter.cpp:
    - TEST(Spice, InverterNetlist): 验证 SPICE 输出格式
    - TEST(Spice, RCFilter): R1+C1 滤波器
    - TEST(Spice, DiffPair): 差分对 SPICE 输出
```

---

### Phase 4: 序列化与文件管理

**目标**：实现完整的 JSON 序列化/反序列化，文件保存/加载无损往返。

**可交付物**：保存电路为 `.msch.json`，重新打开后完整还原。

#### 实现规格

- `Serializer` — 抽象接口
- `JsonSerializer` — JSON 实现，序列化顺序固定，向后兼容
- `FileManager` — 新建/打开/保存/另存为，自动保存，最近文件列表

#### 测试用例 (Phase 4)

```
test_serialization/
  test_json_roundtrip.cpp:
    - EmptyCircuit / SingleComponent / WiresAndLabels / InverterCircuit
    - UnknownFieldsIgnored / MissingOptionalField / MissingRequiredField / FormatVersionCheck
  test_json_files.cpp:
    - LoadInverter / LoadDiffPair / LoadSaveLoad / LoadSymbolLibrary
```

---

### Phase 5: Netlist 导入与自动布局 (N2S)

**目标**：实现从 SPICE netlist 导入电路，自动识别电路功能块、计算元件位置、生成导线。

**可交付物**：File -> Import SPICE Netlist，自动生成完整原理图。

#### 核心算法

**SpiceParser** — SPICE netlist 解析器

**CircuitAnalyzer** — 电路拓扑分析，识别常见模拟功能块：
| 功能块 | 识别条件 |
|--------|---------|
| 差分对 | 两个相同类型 MOS，源极共连，栅极连不同信号 net |
| 电流镜 | 两个+相同类型 MOS，栅极共连，其中一个 G-D 短接 |
| 共源共栅 | 两个相同类型 MOS，上管 S 连下管 D |
| 反相器 | PMOS+NMOS 栅极共连、漏极共连 |
| 源跟随器 | MOS 的 G 为输入，S 为输出 |

#### Sugiyama 算法原理

Sugiyama 算法是层次化图绘制的经典算法，分四步：
1. **去环**：临时反转反馈边使图变为 DAG
2. **分层**：最长路径法分配层号（层=水平位置）
3. **交叉最小化**：重心法调整层内节点顺序
4. **坐标赋值**：确定精确 (x,y) 坐标

#### 我们的改进：功能块级 Sugiyama + 块内模板

两级布局：
1. **块间布局**：对功能块做 Sugiyama 布局
2. **块内布局**：使用固定模板（差分对对称、电流镜横排、共源共栅纵排等）

**SchematicPlacer** — 5 步自动放置算法
**SchematicRouter** — 正交布线 + Label 替代长距离连线 + 电源符号

#### 测试用例 (Phase 5)

包括 SPICE 解析测试、电路分析测试、自动布局测试、自动布线测试、端到端集成测试。

测试用 SPICE 文件：inverter.sp, diff_pair.sp, twostage_opamp.sp, bandgap.sp, folded_cascode.sp

---

### Phase 6: 层次化设计

**目标**：实现 subcircuit 定义与实例化、层次导航、Hierarchical/Flatten 两种显示模式。

#### 两种显示模式的架构设计

**核心原则**：层次化 Circuit 是唯一的数据源（single source of truth），Flatten 视图是派生产物（只读）。

```
Circuit (层次化, 唯一数据源, 所有编辑操作在这里)
    ├──→ Hierarchical View (直接渲染, 可编辑)
    └──→ Flatten View (生成式, 只读)
            ① HierarchyTraverser.traverse(Mode::Flatten)
            ② NetlistImporter 自动布局生成新 Sheet
            ③ 同一实例的器件作为布局组，紧密排列
```

**Flatten 为只读的原因**：一个 SubcircuitDef 可能被实例化多次，编辑某个实例内部的器件无法明确映射回层次源。

**实例聚集约束**：展平时保留每个器件的来源实例信息，布局算法将同一实例的器件作为一个布局组（placement group），组内器件紧密排列，组间保持较大间距。

---

### Phase 7: Symbol 编辑器

**目标**：独立的 Symbol 编辑器 + 库浏览器 + 库文件管理。

---

### Phase 8: DRC 检查

**目标**：Schematic DRC 引擎 + 基础规则集。

规则：FloatingPin, ShortCircuit, UnconnectedNet, DuplicateRef, MissingProperty, PinTypeConflict

---

### Phase 9: 完善与优化

性能优化、复制粘贴、快捷键、打印导出、数字门元件、偏好设置。

---

## 五、阶段依赖

```
Phase 1 基础框架 ──→ Phase 2 Symbol 系统 ──→ Phase 3 导线与连接性
                                                       │
                                                       ▼
                                                 Phase 4 序列化
                                                       │
                                    ┌──────────────────┼──────────────────┐
                                    ▼                  ▼                  ▼
                          Phase 5 N2S导入     Phase 6 层次化     Phase 7 Symbol编辑器
                                                    │                    │
                                                    ▼                    │
                                              Phase 8 DRC  ←────────────┘
                                                    │
                                                    ▼
                                              Phase 9 完善优化
```

Phase 5/6/7 互不依赖，可并行开发。Phase 8 依赖 Phase 6。

---

## 六、验证方法

每个 Phase 完成后：
1. 单元测试全部通过：`ctest --test-dir build`
2. 手动验收：按阶段可交付物的描述进行操作验证
3. 性能检查（Phase 3+）：1000 个元件的测试电路
4. API 测试（Phase 3+）：独立程序链接 libmyschematic.a
5. 序列化往返（Phase 4+）：testdata/ 文件加载→保存→重新加载

### 构建命令

**当前环境（WSL2，无 OpenGL 头文件）**：
```bash
# 配置（lib + tests，不编译 GUI app）
cmake -S . -B build \
    -DCMAKE_PREFIX_PATH="$HOME/Qt/6.5.3/gcc_64;$HOME/gtest-install" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_APP=OFF

# 编译
cmake --build build -j$(nproc)

# 运行测试
ctest --test-dir build --output-on-failure
```

**完整环境（有 Qt Widgets + OpenGL）**：
```bash
# 配置（lib + app + tests）
cmake -S . -B build \
    -DCMAKE_PREFIX_PATH="$HOME/Qt/6.5.3/gcc_64;$HOME/gtest-install" \
    -DCMAKE_BUILD_TYPE=Debug

# 编译
cmake --build build -j$(nproc)

# 运行测试
ctest --test-dir build --output-on-failure

# 启动 GUI 应用
./build/app/myschematic
```

### 依赖安装（无 sudo 权限）

```bash
# CMake (via pip)
pip install cmake

# Qt 6.5.3 (via aqtinstall)
pip install aqtinstall
aqt install-qt linux desktop 6.5.3 gcc_64 -O ~/Qt

# Google Test (从源码编译)
cd /tmp
git clone https://github.com/google/googletest.git --depth 1
cd googletest
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=$HOME/gtest-install
cmake --build build -j$(nproc)
cmake --install build
```
