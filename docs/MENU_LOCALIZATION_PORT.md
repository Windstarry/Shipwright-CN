# SOH 菜单汉化移植规划

> 源：`Shipwright-cn-XPlat-cel-cn-menu` → 目标：`Shipwright-CN`（本仓库）  
> 范围：仅 **ImGui 移植菜单 / 设置界面** 的简体中文本地化；不含游戏内消息、纹理、职员表。

## 一、现状结论

| 项 | Shipwright-CN（目标） | Shipwright-cn-XPlat-cel-cn-menu（源） |
|---|---|---|
| 版本 | 9.2.3，分支 `cn` | 9.2.3，无 `.git`（纯目录副本） |
| 游戏内文本/纹理中文 | 已有 | 有 |
| **ImGui 菜单汉化** | **无** | **完整**（`Localization.*` + `SohGui::L()`） |
| libultraship | 子模块未拉取（空） | 同样为空；URL 为 `roborich/…wind-waker-style` |
| 构建脚本 | 根目录无 `_ninja_*.bat` | 有完整 ninja 脚本 |

源侧菜单汉化是**集中式本地化**：约 4693 条英→简中翻译表 + 绘制路径统一 `L()` 包装 + CJK 字体合并。

## 二、移植范围

### 纳入

1. `soh/soh/SohGui/Localization.h` / `Localization.cpp`（核心翻译表 + `L()` / `SetMenuLanguage` / `IsMenuChinese`）
2. `Menu.cpp`：widget 标签、combo 值、header/sidebar、搜索、`InitElement` 读 CVar
3. `UIWidgets.cpp` / `UIWidgets.hpp`：tooltip、button、radio、combo 等消费点
4. `SohMenuSettings.cpp`：「Menu Language」→ CVar `Menu.Language`（0=EN / 1=中文）
5. `SohGui.cpp`：`Port Menu` / `Modal Window` 可包 `L()`；**GuiWindow 标题绝不能包**
6. 各子窗口 `L()` 包装（约 30+ 文件）
7. `util.cpp`：`sceneNamesChi` / `itemNamesChi` / `questItemNamesChi` + `IsMenuChinese()` 分支
8. `OTRGlobals.cpp`：**CJK 字体合并**（否则菜单中文为方块）+ 提取进度文案 `L()`
9. 零星接入：`Warping.cpp`、`tricks.cpp`、`Anchor/Menu.cpp`、`SohMenuRandomizer.cpp`、`ResolutionEditor.cpp`

### 排除

| 排除项 | 原因 |
|---|---|
| `SohMenuWindWakerStyle.cpp` + `AddMenuWindWakerStyle()` | 风之杖风格功能 |
| `Enhancements/Graphics/WW*`、`ToonLighting` 等 | 同上 |
| `z_staff_CHI.cpp` | 职员表消息，非 ImGui 菜单 |
| `wind-waker-style-docs/`、源树 bat/ninja 残留 | 与移植无关 |
| 替换 libultraship 为 roborich 分支 | 仅服务 WW 着色；**保留 kenix3 子模块** |

## 三、目标架构

```text
CVar Menu.Language ──► Menu::InitElement / Settings 回调
                          │
                          ▼
                 SetMenuLanguage(bool)
                          │
        ┌─────────────────┴─────────────────┐
        ▼                                   ▼
  SohGui::L(en)                      IsMenuChinese()
  查 gChineseTable                   util.cpp 中文场景/道具名
        │
        ▼
  MenuDrawItem / DrawElement / UIWidgets 消费点
```

### 关键约束（源项目已踩坑）

- 翻译键必须与源码英文**拼接结果完全一致**（含 `\n`、`%d %%`）。
- C++ 相邻字符串字面量拼接**不加空格**：`"a." "b"` → `"a.b"`，键不得画蛇添足。
- tooltip 在**消费点**包 `L()`，禁止原地 mutate `options.tooltip`（否则切不回英文）。
- `L("Reset") " (Ctrl+R)"` 非法；须 `std::string(SohGui::L(...)) + "..."`。
- **`AddGuiWindow` 标题 / `.WindowName` 键保持英文**，否则 `GetGuiWindow` 失配 → 窗口空白。
- 扩展翻译只加 `gChineseTable` 条目，不必改调用点。

## 四、文件清单与推荐改法

**原则：按本地化相关 diff 定点套用，不要整文件覆盖**（源树混有 WW 与其它功能改动）。

### 阶段 A — 整文件复制（CN 无同名文件）

| 操作 | 文件 |
|---|---|
| 新增 | `soh/soh/SohGui/Localization.h` |
| 新增 | `soh/soh/SohGui/Localization.cpp` |

CMake 已有 `file(GLOB_RECURSE soh__ … "soh/*.cpp")`，无需改 CMakeLists。

### 阶段 B — 核心接入（定点 patch）

| 文件 | 要点 |
|---|---|
| `SohGui/Menu.cpp` | include；`InitElement` 读 `Menu.Language`；`displayName = L(widget.name)`；combo/header/sidebar/搜索/Quit/Reset |
| `SohGui/UIWidgets.cpp` | Button/Text/Slider/Color/Tooltip 等 `L()` |
| `SohGui/UIWidgets.hpp` | Checkbox/Radio/Combobox/tooltip |
| `SohGui/SohMenuSettings.cpp` | `menuLanguageOptions` +「Menu Language」控件与回调 |
| `SohGui/SohGui.cpp` | include；`Port Menu`/`Modal Window` 包 `L()`；其余窗口标题保持英文 |
| `OTRGlobals.cpp` | CJK merge + `Extracting…` 的 `L()`（**只取此部分，不整文件覆盖**） |

### 阶段 C — 子窗口 `L()` 包装

- `Enhancements/audio/AudioEditor.cpp`
- `Enhancements/debugger/*`（含 `debugSaveEditor`）
- `Enhancements/controls/SohInputEditorWindow.cpp`、`InputViewer.cpp`（combo 选项翻译辅助函数）
- `Enhancements/cosmetics/CosmeticsEditor.cpp`
- `Enhancements/randomizer/*` trackers + `Plandomizer` + `SohMenuRandomizer.cpp`
- `Enhancements/Presets/Presets.cpp`、`gameplaystats.cpp`、`timesplits`、`TimeDisplay`、`mod_menu`
- `Network/Anchor/Menu.cpp`、`AnchorRoomWindow.cpp`
- `SohGui/ResolutionEditor.cpp`、`Enhancements/Warping.cpp`、`randomizer/tricks.cpp`

### 阶段 D — 名称表

| 文件 | 操作 |
|---|---|
| `soh/soh/util.cpp` | 加入三个 `*Chi` 表 + `IsMenuChinese()` 分支 |

### 阶段 E — 运行时资源

- **CJK 字体加载顺序**（`OTRGlobals::CreateFontWithSize`）：
  1. exe 旁 `DroidSansFallback.ttf`（可选，覆盖最全）
  2. `soh.o2r` 资源 `fonts/DroidSansFallback.ttf`（已放入 `soh/assets/custom/fonts/`，打包后自动可用）
  3. `soh.o2r` 资源 `fonts/NotoSansJP-Regular.ttf`（日文子集，简中覆盖不全，仅兜底）
- 另有标点二次合并（`NotoSansJP` 的 General Punctuation），修复弯引号 `“ ”` 等。
- 重建 `soh.o2r` 后新字体才会进包（`GenerateSohOtr` / CMake custom-assets）。

## 五、执行顺序

```text
0. 前置：git 干净；需要编译时 submodule update（kenix3 libultraship）
1. 拷贝 Localization.h/cpp
2. 接入 Menu.cpp + UIWidgets + SohMenuSettings + SohGui.cpp
3. OTRGlobals CJK 字体合并
4. 批量子窗口 L() 包装（阶段 C）
5. util.cpp 中文名表（阶段 D）
6. 编译 Release（勿用 Debug 试玩）
7. 验收
8. 可选：README 补「菜单语言」说明
```

## 六、验收清单

1. **编译**：新增/改动文件零错误；Release 出 `soh.exe`
2. **开关**：Settings → Languages → Menu Language 即时切换；重启后 CVar 保持
3. **覆盖面（中文态）**：顶层 header / 侧边栏；Settings 全量 widget + tooltip；各菜单侧边栏与标题；搜索结果、Quit/Reset/Close；Tracker / Dev Tools 场景与道具名
4. **回退**：切回英文无残留中文；未收录键显示英文
5. **窗口**：Console / Actor Viewer 等打开不空白
6. **字体**：中文笔画完整；缺外部 TTF 时有回退
7. **回归**：游戏内 Language=Chinese 消息/纹理不受影响（与菜单语言正交）

## 七、风险与对策

| 风险 | 对策 |
|---|---|
| 整文件覆盖带入 WW/其它功能 | 只 patch 本地化 diff；WW 文件不进范围 |
| 翻译键不匹配 | 键按拼接后原文；抽检长 tooltip |
| GuiWindow 空白 | 标题保持英文 |
| CJK 方块字 | 必须移植 `OTRGlobals` mergeCJK |
| 子模块为空无法编译 | 先 `git submodule update --init`（保留 kenix3） |
| 源树无 git | 用 `git diff --no-index` 或按清单手工对齐 |

## 八、批次提交建议

1. `feat(menu): add SohGui Localization core + Menu Language CVar`
2. `feat(menu): CJK font merge in ImGui + extract progress L()`
3. `feat(menu): localize UIWidgets/Menu draw paths`
4. `feat(menu): wrap L() in enhancement/dev/randomizer/network windows`
5. `feat(menu): Chinese scene/item/quest names when menu language is Chinese`
