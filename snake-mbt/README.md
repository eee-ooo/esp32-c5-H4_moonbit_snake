# snake-mbt —— 贪吃蛇(MoonBit 源码)

本目录是设备上运行的贪吃蛇游戏的 **MoonBit 源码**。它**不参与 ESP-IDF 构建**,用途有两个:

- 阅读游戏逻辑(输入处理、转向门禁、碰撞判定、绘制)
- 修改游戏后,重新生成"卡带"并烧录

## 与固件的关系

```
snake-mbt/*.mbt ──moon build──▶ snake.wasm ──xxd──▶ components/game_app/gamecard.c ──▶ 固件
   (本目录)                      (中间产物)            (卡带:C 字节数组)
```

固件里运行的是**编译好的卡带**,因此:

- **不改游戏**时,本目录只是源码参考,不参与构建;
- **改游戏**时,按下节流程重新生成 `gamecard.c` 即可。

## 重新生成卡带(需要 MoonBit 工具链)

1. 安装 MoonBit 工具链(提供 `moon` 命令)——本仓库**不包含**工具链;
2. 编译为 WebAssembly:

   ```
   moon build -C ./snake-mbt --target wasm
   ```

3. 把产物转成 C 数组,覆盖工程里的卡带:

   ```
   xxd -n __game_card -i ./snake-mbt/target/wasm/release/build/snake.wasm > ./components/game_app/gamecard.c
   ```

4. 重新 `idf.py build` 并烧录。

## 依赖

- `moonbitlang/wasm4`(WASM-4 的 MoonBit 绑定,构建时由 `moon` 自动拉取,见 `moon.mod.json`)

## 源码导航

| 文件 | 内容 |
|---|---|
| `main.mbt` | 入口:`start()` 设置 4 色调色板;`upd()` 每帧调用 `game.update()` |
| `game.mbt` | 状态机:读手柄、刷新果子、绘制;`frame_count % 30` 决定移动节奏(约 0.5 秒一步) |
| `snake.mbt` | 蛇本体:移动、`up/down/left/right`(带方向门禁)、自身碰撞判定 |

> **方向门禁**:只有横向移动时才接受上/下,纵向移动时才接受左/右。宿主侧"两个按键做相对转向"
> 的实现正是为适配这条约束(见 `components/game_app/control.c`)。

## 许可

本目录源码与 `LICENSE` 均来自上游 MoonBit WASM-4 示例工程(保持原样)。

源文件头为**双版权署名**:

```
//  Copyright 2024 Bruno Garcia
//  Copyright 2024 International Digital Economy Academy
```

含义:贪吃蛇的玩法与示例代码由 **WASM-4 的作者 Bruno Garcia** 创作,
**MoonBit 团队**将其改写为 MoonBit 版本(并声明为 Apache-2.0)。
完整的第三方许可清单见仓库根目录 [`THIRD-PARTY-NOTICES.md`](../THIRD-PARTY-NOTICES.md)。
