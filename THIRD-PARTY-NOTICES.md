# 第三方代码与许可说明(Third-Party Notices)

本仓库包含以下第三方代码,其著作权归各原作者所有,按各自许可分发。
**再分发本仓库(或其衍生作品)时,请保留下列许可文件与版权声明。**

| 组件 | 位置 | 许可 | 许可文件 |
|---|---|---|---|
| WAMR(WebAssembly Micro Runtime) | `components/wamr/` | Apache-2.0(含 LLVM 例外) | `components/wamr/LICENSE`、`components/wamr/ATTRIBUTIONS.md`,以及其自带的第三方许可(如 `LICENSE_ZYDIS`、`LICENSE_ASMJIT`、`LICENSE_LIBUV` 等) |
| WASM-4 本地运行时 | `components/game_app/`(`runtime.c`、`framebuffer.c`、`util.c`、`strnlen.c` 等) | ISC 风格许可,© Bruno Garcia | `LICENSES/LICENSE-WASM4.txt` |
| 上游 ESP32 示例工程(本项目据此移植) | `components/game_app/`(`display.c`、`wamr.c`、`control.c` 等) | MIT,© 2024 lijunchen | `LICENSES/LICENSE-moonbit-wasm4-esp32.txt` |
| 贪吃蛇游戏源码(MoonBit) | `snake-mbt/` | Apache-2.0 | `snake-mbt/LICENSE` |
| 游戏卡带(编译产物) | `components/game_app/gamecard.c` | 由上述 MoonBit 源码编译生成 | 同上 |

## 备注

- WASM-4 是一套"幻想游戏机"规范及其运行时实现;游戏逻辑与运行时接口的著作权归其作者所有。
- 本项目的其余部分(`components/BSP`、`components/cli`、`components/wifi_app`、`components/http_app`、`main/`)为工程自研代码。
