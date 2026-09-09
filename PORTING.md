# ESP32-C5 porting notes

`../moonbit/moonbit-wasm4-esp32-live` is the unchanged upstream reference
project. Its parent commit is `34c14cc849d638896667e3dfdc658d244e6d1916`
and its WAMR submodule is pinned to
`21330990a8f5963dd09d81e491ca4a34f7196ab1`.

This project keeps the upstream WAMR ESP-IDF integration in
`components/wamr/build-scripts/esp-idf/wamr/CMakeLists.txt`. The local
adaptation is limited to the MZ56-ESP32-C5 board, the existing CLI, the LCD
driver, and the reduced-memory game display path. WAMR source selection and
architecture glue are owned by the upstream CMake/Kconfig files.

The game starts at boot after LCD initialization and continues independently
of the USB-Serial-JTAG console. The `game` command is idempotent and only
reports that the already-running game is active. `reset reason=` in the boot
log is used to distinguish a board reset from a console reconnect problem.
