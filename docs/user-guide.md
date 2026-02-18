# MySchematic 用户指南

MySchematic 是一款面向模拟电路设计的原理图编辑器，对标 Cadence Virtuoso Schematic Editor，支持大规模电路（10 万+ 器件）、层次化设计、SPICE netlist 导出与导入。

> **当前开发状态**：Phase 1 已完成（2026-02-18）。核心数据模型库 `libmyschematic.a` 可用，GUI 应用代码已写好但需要完整 Qt Widgets 环境。本指南描述的功能中，部分尚在后续阶段实现中，请参考 [development-plan.md](development-plan.md) 中的进度表了解各功能的可用状态。

---

## 目录

1. [安装与构建](#1-安装与构建)
2. [快速入门](#2-快速入门)
3. [界面概览](#3-界面概览)
4. [基本操作](#4-基本操作)
5. [元件放置与编辑](#5-元件放置与编辑)
6. [导线连接](#6-导线连接)
7. [网络标号与电源符号](#7-网络标号与电源符号)
8. [属性编辑](#8-属性编辑)
9. [层次化设计](#9-层次化设计)
10. [SPICE Netlist 导出](#10-spice-netlist-导出)
11. [SPICE Netlist 导入（N2S）](#11-spice-netlist-导入n2s)
12. [DRC 检查](#12-drc-检查)
13. [Symbol 编辑器](#13-symbol-编辑器)
14. [文件管理](#14-文件管理)
15. [快捷键一览](#15-快捷键一览)
16. [C++ API 集成](#16-c-api-集成)
17. [文件格式参考](#17-文件格式参考)

---

## 1. 安装与构建

### 系统要求

- **操作系统**：Linux（主要支持）、Windows（兼容）
- **编译器**：GCC 9+ 或 Clang 10+（需支持 C++17）
- **Qt**：Qt 6.5+（需要 Core, Widgets, Gui 模块）
- **CMake**：3.16+
- **测试框架**：Google Test（可选，构建测试时需要）

### 依赖安装

如果系统没有 sudo 权限，可使用以下方式安装依赖：

```bash
# CMake (via pip)
pip install cmake

# Qt 6.5.3 (via aqtinstall)
pip install aqtinstall
aqt install-qt linux desktop 6.5.3 gcc_64 -O ~/Qt

# Google Test (从源码编译)
cd /tmp && git clone https://github.com/google/googletest.git --depth 1
cd googletest
cmake -S . -B build -DCMAKE_INSTALL_PREFIX=$HOME/gtest-install
cmake --build build -j$(nproc) && cmake --install build
```

### 构建步骤

```bash
# 克隆项目
git clone <repo-url> myschematic
cd myschematic

# 配置（指定 Qt 和 GTest 安装路径）
cmake -S . -B build \
    -DCMAKE_PREFIX_PATH="$HOME/Qt/6.5.3/gcc_64;$HOME/gtest-install" \
    -DCMAKE_BUILD_TYPE=Debug

# 如果没有 OpenGL 头文件（如 WSL2 环境），关闭 GUI 应用编译
cmake -S . -B build \
    -DCMAKE_PREFIX_PATH="$HOME/Qt/6.5.3/gcc_64;$HOME/gtest-install" \
    -DCMAKE_BUILD_TYPE=Debug \
    -DBUILD_APP=OFF

# 编译
cmake --build build -j$(nproc)

# 运行测试
ctest --test-dir build --output-on-failure

# 启动 GUI 应用（仅在 BUILD_APP=ON 时可用）
./build/app/myschematic
```

### 构建选项

| CMake 选项 | 默认值 | 说明 |
|-----------|--------|------|
| `CMAKE_BUILD_TYPE` | `Debug` | 构建类型：Debug / Release / RelWithDebInfo |
| `BUILD_TESTS` | `ON` | 是否构建测试 |
| `BUILD_APP` | `ON` | 是否构建 GUI 应用（需要 Qt Widgets + OpenGL） |
| `CMAKE_PREFIX_PATH` | — | Qt6 和 GTest 安装路径，分号分隔 |

---

## 2. 快速入门

### 创建第一个电路：CMOS 反相器

1. 启动 MySchematic：`./myschematic`
2. 新建文件：**File -> New** (Ctrl+N)
3. 放置 PMOS：
   - 打开元件面板（左侧 Component Palette）
   - 在 MOSFET 分类下双击 **pmos4**（或按 **I** 键进入放置模式）
   - 在画布上点击放置
4. 放置 NMOS：同上，选择 **nmos4**
5. 连接导线：
   - 按 **W** 键进入画线模式
   - 点击 PMOS 的 D（漏极）引脚
   - 点击 NMOS 的 D（漏极）引脚
   - 继续连接其他引脚
6. 添加电源符号：
   - 从元件面板选择 **VDD** 和 **GND**
   - 放置在 PMOS 的 S（源极）和 NMOS 的 S（源极）附近
   - 用导线连接
7. 添加网络标号：
   - 按 **L** 键进入标号模式
   - 在输入端放置标号 "IN"
   - 在输出端放置标号 "OUT"
8. 编辑属性：
   - 双击 PMOS，设置 W=2u, L=100n, model=pch
   - 双击 NMOS，设置 W=1u, L=100n, model=nch
9. 导出 SPICE：**File -> Export SPICE Netlist**
10. 保存：**File -> Save** (Ctrl+S)

---

## 3. 界面概览

```
┌─────────────────────────────────────────────────────────────┐
│  File  Edit  View  Tools  Help                    [菜单栏] │
├─────────────────────────────────────────────────────────────┤
│  [选择] [放置] [画线] [标号] [Undo] [Redo] [Zoom] [工具栏] │
├──────────┬─────────────────────────────────┬────────────────┤
│          │                                 │                │
│ Component│                                 │  Property      │
│ Palette  │      Schematic Canvas           │  Editor        │
│          │      (主画布区域)                │                │
│          │                                 │                │
│──────────│                                 │────────────────│
│          │                                 │                │
│ Hierarchy│                                 │  DRC Results   │
│ Panel    │                                 │  Panel         │
│          │                                 │                │
├──────────┴─────────────────────────────────┴────────────────┤
│  [状态栏: 坐标, 缩放级别, 当前工具, 层次路径]              │
└─────────────────────────────────────────────────────────────┘
```

### 面板说明

| 面板 | 位置 | 功能 |
|------|------|------|
| **Component Palette** | 左上 | 显示可用元件库，按类别分组，双击或拖拽放置 |
| **Hierarchy Panel** | 左下 | 显示电路层次树，点击切换层级 |
| **Property Editor** | 右上 | 显示/编辑选中元件的属性 |
| **DRC Results** | 右下 | 显示 DRC 检查结果，双击定位到违规位置 |

---

## 4. 基本操作

### 画布导航

| 操作 | 方式 |
|------|------|
| **平移** | 鼠标中键拖动 |
| **缩放** | 鼠标滚轮（以鼠标位置为中心） |
| **适合窗口** | Ctrl+0 或 **F** 键 |

### 选择

| 操作 | 方式 |
|------|------|
| **单选** | 点击元件 |
| **多选** | Shift + 点击 |
| **框选** | 在空白处拖拽矩形 |
| **全选** | Ctrl+A |
| **取消选择** | 点击空白处或 ESC |

### 编辑操作

| 操作 | 方式 |
|------|------|
| **移动** | 选中后拖拽，或按 **M** 键 |
| **旋转** | 选中后按 **R** 键（逆时针 90 度） |
| **镜像** | 选中后按 **X** 键（水平翻转） |
| **删除** | 选中后按 **Delete** 键 |
| **复制** | Ctrl+C |
| **粘贴** | Ctrl+V |
| **剪切** | Ctrl+X |
| **撤销** | Ctrl+Z |
| **重做** | Ctrl+Y 或 Ctrl+Shift+Z |

---

## 5. 元件放置与编辑

### 放置元件

1. **从元件面板**：在左侧 Component Palette 中双击元件，进入放置模式
2. **快捷键**：按 **I** 键，打开元件选择对话框
3. **放置模式**：
   - 鼠标跟随显示元件预览
   - 点击画布放置元件
   - 放置前可按 **R** 旋转、**X** 镜像
   - 按 **ESC** 退出放置模式

### 内置元件

#### MOSFET

| 元件 | 引脚 | 常用属性 |
|------|------|---------|
| **nmos4** (N-Channel MOSFET) | G(栅极), D(漏极), S(源极), B(衬底) | W, L, model, nf, m |
| **pmos4** (P-Channel MOSFET) | G, D, S, B | W, L, model, nf, m |

#### 无源器件

| 元件 | 引脚 | 常用属性 |
|------|------|---------|
| **resistor** (电阻) | P(正极), N(负极) | value |
| **capacitor** (电容) | P, N | value |
| **inductor** (电感) | P, N | value |

#### 二极管与 BJT

| 元件 | 引脚 | 常用属性 |
|------|------|---------|
| **diode** | A(阳极), K(阴极) | model |
| **npn** (NPN BJT) | B(基极), C(集电极), E(发射极) | model |
| **pnp** (PNP BJT) | B, C, E | model |

#### 电源

| 元件 | 引脚 | 常用属性 |
|------|------|---------|
| **vsource** (电压源) | P, N | value |
| **isource** (电流源) | P, N | value |

#### 受控源

| 元件 | 引脚 | 常用属性 |
|------|------|---------|
| **vcvs** (电压控制电压源) | NP, NN, CP, CN | gain |
| **vccs** (电压控制电流源) | NP, NN, CP, CN | gm |
| **ccvs** (电流控制电压源) | NP, NN, CP, CN | rm |
| **cccs** (电流控制电流源) | NP, NN, CP, CN | gain |

#### 电源符号

| 元件 | 说明 |
|------|------|
| **gnd** | 接地符号，隐式连接到全局 GND 网络 |
| **vdd** | 电源符号，隐式连接到全局 VDD 网络 |

---

## 6. 导线连接

### 画导线

1. 按 **W** 键进入画线模式
2. 点击起点（会自动 snap 到最近的引脚或网格点）
3. 移动鼠标，实时预览正交路径（默认 L 型）
4. 按**空格键**切换路径方向（先水平后垂直 / 先垂直后水平）
5. 点击添加拐点
6. 双击或点击引脚结束导线
7. 按 **ESC** 取消当前导线

### 连接规则

- 导线端点与引脚位置重合时自动连接
- 两条导线在同一点相交时，需要 **Junction**（交叉点）标记才视为连接
- 导线 T 型交汇时自动添加 Junction
- 十字交叉不自动连接（避免误连）

### Junction（交叉点）

- 三条或更多导线在同一点汇合时，显示为实心小圆点
- 系统在画导线时会自动添加 Junction
- 也可手动放置以明确标记连接关系

---

## 7. 网络标号与电源符号

### 网络标号 (Label)

网络标号用于命名导线网络，相同名称的标号表示电气连接（即使没有导线直接相连）。

1. 按 **L** 键进入标号模式
2. 输入标号名称（如 VIN, VOUT, BIAS）
3. 点击导线上的位置放置
4. 按 **ESC** 退出

**用途**：
- 给关键网络命名，方便识别
- 替代长距离导线连接（两端放置相同名称的标号即可）
- 在 SPICE netlist 中作为节点名使用

### 电源符号 (Power Symbol)

VDD 和 GND 是特殊的全局网络：
- **GND** 符号放置后自动连接到全局 "0" 网络
- **VDD** 符号放置后自动连接到全局 "VDD" 网络
- 无需手动画导线将所有电源引脚连到一起

---

## 8. 属性编辑

### 编辑元件属性

1. **双击**元件，打开属性对话框
2. 或选中元件后按 **Q** 键
3. 或在右侧 **Property Editor** 面板中直接编辑

### 常见属性

| 属性 | 适用元件 | 说明 | 示例 |
|------|---------|------|------|
| `instanceName` | 所有 | 实例名（SPICE 中的器件名） | M1, R1, C1 |
| `model` | MOSFET, BJT, Diode | SPICE 模型名 | nch, pch |
| `W` | MOSFET | 沟道宽度 | 10u, 1u |
| `L` | MOSFET | 沟道长度 | 100n, 1u |
| `nf` | MOSFET | 手指数 | 1, 2, 4 |
| `m` | MOSFET | 并联倍数 | 1, 2 |
| `value` | R, C, L | 元件值 | 1k, 1p, 10n |

### 工程计数法

属性值支持标准工程计数后缀：

| 后缀 | 含义 | 示例 |
|------|------|------|
| T | 10^12 | 1T = 1e12 |
| G | 10^9 | 1G = 1e9 |
| M (大写) | 10^6 | 1M = 1e6 |
| k | 10^3 | 1k = 1000 |
| m (小写) | 10^-3 | 1m = 0.001 |
| u | 10^-6 | 1u = 1e-6 |
| n | 10^-9 | 100n = 1e-7 |
| p | 10^-12 | 1p = 1e-12 |
| f | 10^-15 | 1f = 1e-15 |

---

## 9. 层次化设计

### 概念

层次化设计允许将复杂电路分解为可复用的子电路（subcircuit），类似于编程中的函数。

### 创建子电路

1. **Tools -> Create Subcircuit**
2. 输入子电路名称（如 "diff_pair"）
3. 定义端口（ports）：名称和方向（Input/Output/InOut）
4. 进入子电路内部 Sheet 编辑器
5. 在内部放置元件和导线
6. 放置 Port 标记端口位置

### 实例化子电路

1. 在 Component Palette 中找到已定义的子电路
2. 双击或拖拽到主 Sheet
3. 子电路显示为带引脚的矩形方块
4. 可设置实例参数覆盖默认值

### 层次导航

| 操作 | 方式 |
|------|------|
| **进入子电路** (Descend) | 双击子电路实例 |
| **返回上层** (Ascend) | Hierarchy Panel 中点击上层，或按返回键 |
| **查看层次路径** | 状态栏显示当前路径，如 `top / X1(diff_pair)` |

### Hierarchical 与 Flatten 视图

| 视图 | 说明 | 可编辑 |
|------|------|--------|
| **Hierarchical** | 默认视图，子电路显示为方块符号 | 是 |
| **Flatten** | 展平视图，所有层次展开为平面电路 | 否（只读） |

切换方式：**View -> Hierarchical View / Flatten View**

Flatten 视图特点：
- 所有子电路实例展开为单个器件
- 来自同一实例的器件聚集在一起显示
- 可能绘制虚线边框标注实例名称
- 适合审查完整电路和验证连接性
- 不可编辑，修改需在 Hierarchical 视图中进行

---

## 10. SPICE Netlist 导出

### 导出操作

**File -> Export SPICE Netlist** (或使用菜单快捷键)

### 导出选项

| 选项 | 说明 |
|------|------|
| **Dialect** | SPICE 方言：Generic / HSpice / Spectre / LTSpice |
| **Mode** | Flatten（展平为单一网表）/ Hierarchical（保留 .SUBCKT 层次） |
| **Title** | 网表标题行 |

### Hierarchical 导出示例

```spice
* Two-Stage Op-Amp

.SUBCKT diff_pair INP INM OUTP OUTM VDD VSS
M1 OUTP INP TAIL VSS nch W=10u L=1u
M2 OUTM INM TAIL VSS nch W=10u L=1u
M3 TAIL BIAS VSS VSS nch W=20u L=1u
.ENDS diff_pair

X1 INP INM NET1 NET2 VDD VSS diff_pair
M6 VOUT NET2 VDD VDD pch W=40u L=1u
M7 VOUT BIAS VSS VSS nch W=20u L=1u
CC VOUT NET2 1p

.END
```

### Flatten 导出示例

```spice
* Two-Stage Op-Amp (Flattened)

M1 NET1 INP TAIL 0 nch W=10u L=1u
M2 NET2 INM TAIL 0 nch W=10u L=1u
M3 TAIL BIAS 0 0 nch W=20u L=1u
M4 NET1 NET1 VDD VDD pch W=10u L=1u
M5 NET2 NET1 VDD VDD pch W=10u L=1u
M6 VOUT NET2 VDD VDD pch W=40u L=1u
M7 VOUT BIAS 0 0 nch W=20u L=1u
CC VOUT NET2 1p

.END
```

---

## 11. SPICE Netlist 导入（N2S）

### 功能说明

从 SPICE netlist 文件导入电路，自动生成带有位置信息的原理图。系统会：
1. 解析 SPICE netlist 中的器件和连接关系
2. 自动识别电路功能块（差分对、电流镜、共源共栅等）
3. 使用智能布局算法计算元件位置
4. 自动生成导线、标号和电源符号

### 导入操作

**File -> Import SPICE Netlist...**

选择 `.sp`、`.spice`、`.cir` 或 `.net` 文件。

### 导入选项

| 选项 | 默认值 | 说明 |
|------|--------|------|
| Grid Size | 10.0 | 网格对齐精度 |
| Component Spacing | 80.0 | 元件间最小间距 |
| Layer Spacing | 200.0 | 信号级间水平距离 |
| Use Labels for Long Nets | true | 长距离连接使用标号代替导线 |
| Long Net Threshold | 500 | 标号替代导线的距离阈值 |
| Recognize Patterns | true | 是否识别功能块 |
| Add Power Symbols | true | VDD/GND 使用电源符号 |

### 布局原则

导入后的原理图遵循模拟电路的阅读惯例：
- **信号流从左到右**：输入端在左侧，输出端在右侧
- **VDD 在上，GND 在下**：电源轨遵循惯例方向
- **差分对对称排列**：两个配对管左右对称
- **电流镜水平排列**：参考管在左，镜像管在右
- **共源共栅纵向堆叠**：共栅管在上，共源管在下
- **功能块间保持间距**：不同功能模块清晰分隔

### 支持的 SPICE 语法

- 器件行：`M1 drain gate source bulk modelname W=1u L=100n`
- 续行符：`+` 开头的行与前一行合并
- 子电路：`.SUBCKT name pin1 pin2 ...` / `.ENDS`
- 参数：`.param name=value`
- 注释：`*` 行首注释，`$` 行内注释
- 包含文件：`.include path`（记录但不递归加载）
- 仿真命令：`.tran`、`.ac` 等被忽略

### 器件类型识别

器件类型由实例名首字母识别：

| 首字母 | 器件类型 |
|--------|---------|
| M | MOSFET |
| R | 电阻 |
| C | 电容 |
| L | 电感 |
| D | 二极管 |
| Q | BJT |
| V | 电压源 |
| I | 电流源 |
| E | VCVS (电压控制电压源) |
| F | CCCS (电流控制电流源) |
| G | VCCS (电压控制电流源) |
| H | CCVS (电流控制电压源) |
| X | 子电路实例 |

---

## 12. DRC 检查

### 运行 DRC

**Tools -> Run DRC** 或使用快捷键

### DRC 规则

| 规则 | 严重级别 | 说明 |
|------|---------|------|
| **Floating Pin** | Error | 非 NC 类型的引脚未连接导线 |
| **Short Circuit** | Error | 两个 Output 类型引脚直接相连 |
| **Unconnected Net** | Warning | 网络只有 1 个连接点（悬空线） |
| **Duplicate Reference** | Error | 多个元件使用相同的实例名 |
| **Missing Property** | Error | 必填属性为空值 |
| **Pin Type Conflict** | Warning | 引脚电气类型冲突 |

### 查看结果

- DRC 结果显示在右下方的 **DRC Results** 面板
- 每条违规显示：严重级别图标、规则名、描述、位置
- **双击**违规条目可自动定位到画布上的问题位置
- 违规位置会在画布上叠加红色/黄色标记

### 规则配置

可以启用/禁用特定规则：**Tools -> DRC Settings**

---

## 13. Symbol 编辑器

### 打开 Symbol 编辑器

- **Tools -> Symbol Editor** 打开独立的 Symbol 编辑窗口
- 或在 Library Browser 中双击已有 symbol

### 创建自定义 Symbol

1. 在 Symbol 编辑器中使用绘图工具绘制符号图形：
   - **Line Tool** — 直线
   - **Rectangle Tool** — 矩形
   - **Circle Tool** — 圆
   - **Arc Tool** — 圆弧
   - **Polyline Tool** — 折线/多边形
   - **Text Tool** — 文字标注
2. 使用 **Pin Place Tool** 放置引脚，设置：
   - 引脚名称 (如 G, D, S, B)
   - 方向 (Left/Right/Up/Down)
   - 电气类型 (Input/Output/Passive/Power/Bidirectional)
3. 编辑属性模板（Property Templates）：定义元件的可配置参数
4. 编辑 SPICE Template：定义 netlist 导出格式
5. 保存到符号库文件 (`.mschlib.json`)

### Library Browser

- 左侧面板，树形显示：库名 -> 元件列表
- 搜索过滤：快速查找元件
- 拖拽到主画布放置

---

## 14. 文件管理

### 文件类型

| 扩展名 | 说明 |
|--------|------|
| `.msch.json` | 原理图文件（JSON 文本格式） |
| `.mschlib.json` | 符号库文件（JSON 文本格式） |

### 文件操作

| 操作 | 快捷键 | 说明 |
|------|--------|------|
| **新建** | Ctrl+N | 创建空白原理图 |
| **打开** | Ctrl+O | 打开 .msch.json 文件 |
| **保存** | Ctrl+S | 保存当前文件 |
| **另存为** | Ctrl+Shift+S | 保存到新路径 |

### 自动保存

- 默认每 60 秒自动保存到 `.msch.json.autosave` 文件
- 异常退出后重新打开时，系统会提示恢复自动保存的文件
- 可在偏好设置中调整自动保存间隔

### 最近文件

**File -> Recent Files** 显示最近打开的文件列表。

---

## 15. 快捷键一览

### 文件操作

| 快捷键 | 功能 |
|--------|------|
| Ctrl+N | 新建 |
| Ctrl+O | 打开 |
| Ctrl+S | 保存 |
| Ctrl+Shift+S | 另存为 |

### 编辑操作

| 快捷键 | 功能 |
|--------|------|
| Ctrl+Z | 撤销 |
| Ctrl+Y / Ctrl+Shift+Z | 重做 |
| Ctrl+C | 复制 |
| Ctrl+V | 粘贴 |
| Ctrl+X | 剪切 |
| Delete | 删除选中 |
| Ctrl+A | 全选 |

### 工具切换

| 快捷键 | 功能 |
|--------|------|
| I | 放置元件 |
| W | 画导线 |
| L | 放置网络标号 |
| ESC | 取消当前操作 / 回到选择工具 |

### 元件操作

| 快捷键 | 功能 |
|--------|------|
| R | 旋转（逆时针 90 度） |
| X | 水平镜像 |
| M | 移动 |
| Q | 编辑属性 |

### 视图操作

| 快捷键 | 功能 |
|--------|------|
| F / Ctrl+0 | 适合窗口 |
| 滚轮 | 缩放 |
| 中键拖动 | 平移 |

---

## 16. C++ API 集成

MySchematic 的核心功能编译为静态库 `libmyschematic.a`，可被外部程序集成。

### 链接方式

```cmake
# 在你的 CMakeLists.txt 中
find_package(Qt6 REQUIRED COMPONENTS Core)
target_link_libraries(your_app PRIVATE myschematic Qt6::Core)
target_include_directories(your_app PRIVATE path/to/myschematic/lib/include)
```

### API 示例：加载电路并导出 SPICE

```cpp
#include <myschematic/myschematic.h>

int main() {
    myschematic::Circuit circuit;
    circuit.load("my_design.msch.json");

    // 生成网表
    auto netlist = circuit.generateNetlist(
        myschematic::HierarchyTraverser::Mode::Flatten);

    // 导出 SPICE
    myschematic::SpiceExporter exporter;
    exporter.setDialect(myschematic::SpiceExporter::Dialect::HSpice);
    exporter.exportToFile(netlist, "my_design.sp");

    return 0;
}
```

### API 示例：编程创建电路

```cpp
#include <myschematic/myschematic.h>

int main() {
    myschematic::Circuit circuit;
    auto* sheet = circuit.createSheet("top");

    // 放置 NMOS
    auto* nmos = sheet->addComponent(
        circuit.libraryManager()->findSymbol("basic_analog", "nmos4"),
        QPointF(200, 300));
    nmos->setInstanceName("M1");
    nmos->setProperty("W", "10u");
    nmos->setProperty("L", "100n");

    // 放置 PMOS
    auto* pmos = sheet->addComponent(
        circuit.libraryManager()->findSymbol("basic_analog", "pmos4"),
        QPointF(200, 100));
    pmos->setInstanceName("M2");
    pmos->setProperty("W", "20u");
    pmos->setProperty("L", "100n");

    // 画导线连接漏极
    sheet->addWire({QPointF(200, 280), QPointF(200, 120)});

    // 运行 DRC
    myschematic::DrcEngine drc;
    drc.addRule(std::make_unique<myschematic::FloatingPinRule>());
    auto violations = drc.runAll(circuit);

    // 保存
    circuit.save("inverter.msch.json");

    return 0;
}
```

### API 示例：从 SPICE 导入

```cpp
#include <myschematic/myschematic.h>

int main() {
    myschematic::Circuit circuit;
    myschematic::NetlistImporter importer;

    myschematic::NetlistImporter::ImportOptions opts;
    opts.recognizePatterns = true;
    opts.addPowerSymbols = true;

    auto* sheet = importer.importFromSpiceFile(
        "opamp.sp",
        circuit.libraryManager(),
        opts);

    if (sheet) {
        // 导入成功，保存为 schematic
        circuit.save("opamp.msch.json");
    }

    // 查看导入警告
    for (const auto& warning : importer.warnings()) {
        qWarning() << warning;
    }

    return 0;
}
```

---

## 17. 文件格式参考

### 原理图文件 (.msch.json)

```json
{
  "formatVersion": "1.0",
  "generator": "myschematic-1.0.0",
  "createdAt": "2026-02-18T12:00:00Z",
  "modifiedAt": "2026-02-18T12:30:00Z",
  "metadata": {
    "title": "Circuit Name",
    "author": "Designer",
    "description": "Description",
    "revision": "1.0"
  },
  "libraryRefs": [
    { "name": "basic_analog", "path": "symbols/basic_analog.mschlib.json" }
  ],
  "sheets": [ ... ],
  "subcircuitDefs": [ ... ]
}
```

详细格式规范请参考 [development-plan.md](development-plan.md) 中的"Schematic 文件格式规范 (v1.0)"章节。

### 符号库文件 (.mschlib.json)

```json
{
  "formatVersion": "1.0",
  "libraryName": "library_name",
  "symbols": [
    {
      "name": "symbol_name",
      "description": "Description",
      "category": "Category",
      "pins": [ ... ],
      "graphics": [ ... ],
      "propertyTemplates": [ ... ],
      "spiceTemplate": "..."
    }
  ]
}
```

### 格式设计原则

- **UUID 标识所有对象**：避免重命名导致的 diff 噪音
- **坐标值整数化**：网格对齐后坐标为整数
- **确定性排序**：序列化时按 UUID 排序，减少无意义 diff
- **向后兼容**：反序列化忽略未知字段，缺失字段使用默认值
- **Git 友好**：文本格式的 JSON 文件可直接 git diff
