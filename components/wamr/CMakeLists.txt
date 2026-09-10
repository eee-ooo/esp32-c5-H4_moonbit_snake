# Use WAMR's official ESP-IDF component integration.
# This selects the target ABI, architecture invokeNative implementation,
# interpreter, loader, platform and allocator sources from Kconfig.
include(${CMAKE_CURRENT_LIST_DIR}/build-scripts/esp-idf/wamr/CMakeLists.txt)
