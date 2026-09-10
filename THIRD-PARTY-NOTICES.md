# 第三方代码与许可说明(Third-Party Notices)

本仓库包含以下第三方代码,其著作权归各原作者所有,按各自许可分发。
**再分发本仓库(或其衍生作品)时,请保留下列许可文件与版权声明。**

| 组件 | 位置 | 许可 / 版权 | 许可文件 |
|---|---|---|---|
| WAMR(WebAssembly Micro Runtime) | `components/wamr/` | Apache-2.0(含 LLVM 例外) | `components/wamr/LICENSE`、`components/wamr/ATTRIBUTIONS.md`,以及其自带的第三方许可(如 `LICENSE_ZYDIS`、`LICENSE_ASMJIT`、`LICENSE_LIBUV` 等) |
| WASM-4 本地运行时 | `components/game_app/`(`runtime.c`、`framebuffer.c`、`util.c`、`strnlen.c` 等) | ISC 风格许可,© Bruno Garcia | `LICENSES/LICENSE-WASM4.txt` |
| 上游 ESP32 示例工程(本项目据此移植) | `components/game_app/`(`display.c`、`wamr.c`、`control.c` 等) | MIT,© 2024 lijunchen | `LICENSES/LICENSE-moonbit-wasm4-esp32.txt` |
| 贪吃蛇游戏源码(MoonBit) | `snake-mbt/` | **双版权署名**:© 2024 Bruno Garcia、© 2024 International Digital Economy Academy;由 WASM-4 官方文档示例**改写**而来 | `snake-mbt/LICENSE`(Apache-2.0);注意其 `moon.mod.json` 中声明的许可为 ISC |
| 游戏卡带(编译产物) | `components/game_app/gamecard.c` | 由上述 MoonBit 源码编译生成 | 同上 |
| `moonbitlang/wasm4`(MoonBit 语言绑定库) | **不在本仓库**——仅在重建卡带时由 `moon` 从包仓库拉取 | Apache-2.0,MoonBit 团队 | 见该包自身仓库 |

## 备注

- **WASM-4 是独立的"幻想游戏机"平台**(规格 + ABI + 运行时 + 工具链,© Bruno Garcia,ISC 许可);
  `moonbitlang/wasm4` 是 **MoonBit 团队为自家语言编写的 WASM-4 绑定库**(把裸 wasm import 封装成类型化 API),
  两者不是同一项目,本仓库也**不包含**该绑定库。
- WASM-4 的游戏逻辑与运行时接口著作权归其作者所有;游戏必须遵循的 ABI 契约见 `snake-mbt/moon.pkg.json`。
- 本项目的其余部分(`components/BSP`、`components/cli`、`components/wifi_app`、`components/http_app`、`main/`)为工程自研代码。
