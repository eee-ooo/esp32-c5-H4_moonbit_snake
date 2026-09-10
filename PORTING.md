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

## WAMR version selection

The vendored WAMR corresponds to the upstream `main` snapshot of 2024-09-24
(version 2.1.2 in `core/version.h`) — the same generation as the reference
example. Using a recent release caused a large amount of churn (source layout,
macro names and defaults had all changed), so the example-era snapshot is used.

Two notes on that snapshot:

- Its `idf_component.yml` declared the supported target list **without
  `esp32c5`** (the chip did not exist when the file was written), which makes
  CMake fail with `not compatible with target esp32c5`. The file is a
  declaration only and has been removed; WAMR itself runs fine on the C5.
- `components/wamr/CMakeLists.txt` is reduced to a single `include(...)` of the
  upstream ESP-IDF integration script, so source selection, ABI and platform
  glue remain owned by upstream.

WAMR is vendored as plain files (no submodule pointer), so a fresh clone builds
without fetching anything else.

## MoonBit game sources

`snake-mbt/` holds the MoonBit sources of the cartridge the firmware runs. They
are not part of the ESP-IDF build; see `snake-mbt/README.md` for the cartridge
rebuild flow.
