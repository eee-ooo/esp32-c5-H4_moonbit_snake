#include "wamr.h"
#include "runtime.h"
#include <stddef.h>

void wrap_w4_runtimeBlit(wasm_exec_env_t exec_env, const uint8_t* sprite, int x, int y, int width, int height, int flags) {
    // printf("Call wrap_w4_runtimeBlit\n");
    w4_runtimeBlit(sprite, x, y, width, height, flags);
}
void wrap_w4_runtimeBlitSub(wasm_exec_env_t exec_env, const uint8_t* sprite, int x, int y, int width, int height, int srcX, int srcY, int stride, int flags) {
    // printf("Call wrap_w4_runtimeBlitSub\n");
    w4_runtimeBlitSub(sprite, x, y, width, height, srcX, srcY, stride, flags);
}
void wrap_w4_runtimeLine(wasm_exec_env_t exec_env, int x1, int y1, int x2, int y2) {
    // printf("Call wrap_w4_runtimeLine\n");
    w4_runtimeLine(x1, y1, x2, y2);
}
void wrap_w4_runtimeHLine(wasm_exec_env_t exec_env, int x, int y, int len) {
    // printf("Call wrap_w4_runtimeHLine\n");
    w4_runtimeHLine(x, y, len);
}
void wrap_w4_runtimeVLine(wasm_exec_env_t exec_env, int x, int y, int len) {
    // printf("Call wrap_w4_runtimeVLine\n");
    w4_runtimeVLine(x, y, len);
}
void wrap_w4_runtimeOval(wasm_exec_env_t exec_env, int x, int y, int width, int height) {
    // printf("Call wrap_w4_runtimeOval\n");
    w4_runtimeOval(x, y, width, height);
}
void wrap_w4_runtimeRect(wasm_exec_env_t exec_env, int x, int y, int width, int height) {
    static bool logged;
    if (!logged) {
        printf("WASM rect native call: x=%d y=%d w=%d h=%d\n",
               x, y, width, height);
        logged = true;
    }
    w4_runtimeRect(x, y, width, height);
}
void wrap_w4_runtimeText(wasm_exec_env_t exec_env, const uint8_t* str, int x, int y) {
    // printf("Call wrap_w4_runtimeText\n");
    w4_runtimeText(str, x, y);
}
void wrap_w4_runtimeTextUtf8(wasm_exec_env_t exec_env, const uint8_t* str, int byteLength, int x, int y) {
    // printf("Call wrap_w4_runtimeTextUtf8\n");
    w4_runtimeTextUtf8(str, byteLength, x, y);
}
void wrap_w4_runtimeTextUtf16(wasm_exec_env_t exec_env, const uint16_t* str, int byteLength, int x, int y) {
    // printf("Call wrap_w4_runtimeTextUtf16\n");
    w4_runtimeTextUtf16(str, byteLength, x, y);
}
void wrap_w4_runtimeTone(wasm_exec_env_t exec_env, int frequency, int duration, int volume, int flags) {
    // printf("Call wrap_w4_runtimeTone\n");
    w4_runtimeTone(frequency, duration, volume, flags);
}
int wrap_w4_runtimeDiskr(wasm_exec_env_t exec_env, uint8_t* dest, int size) {
    // printf("Call wrap_w4_runtimeDiskr\n");
    return w4_runtimeDiskr(dest, size);
}
int wrap_w4_runtimeDiskw(wasm_exec_env_t exec_env, const uint8_t* src, int size) {
    // printf("Call wrap_w4_runtimeDiskw\n");
    return w4_runtimeDiskw(src, size);
}
void wrap_w4_runtimeTrace(wasm_exec_env_t exec_env, const uint8_t* str) {
    // printf("Call wrap_w4_runtimeTrace\n");
    w4_runtimeTrace(str);
}
void wrap_w4_runtimeTraceUtf8(wasm_exec_env_t exec_env, const uint8_t* str, int byteLength) {
    // printf("Call wrap_w4_runtimeTraceUtf8\n");
    w4_runtimeTraceUtf8(str, byteLength);
}
void wrap_w4_runtimeTraceUtf16(wasm_exec_env_t exec_env, const uint16_t* str, int byteLength) {
    // printf("Call wrap_w4_runtimeTraceUtf16\n");
    w4_runtimeTraceUtf16(str, byteLength);
}
void wrap_w4_runtimeTracef(wasm_exec_env_t exec_env, const uint8_t* str, const void* stack) {
    // printf("Call wrap_w4_runtimeTracef\n");
    w4_runtimeTracef(str, stack);
}

void just_print_int(wasm_exec_env_t exec_env, int a) {
    printf("JUST PRINT INT %d\n", a);
}

static NativeSymbol native_symbols[] =
{
    {
        "just_print_int",
        just_print_int,
        "(i)"
    },
    {
        "blit",
     	wrap_w4_runtimeBlit,
        "(*iiiii)"
    },
    {
        "blitSub",
     	wrap_w4_runtimeBlitSub,
        "(*iiiiiiii)"
    },
    {
        "line",
     	wrap_w4_runtimeLine,
        "(iiii)"
    },
    {
        "hline",
     	wrap_w4_runtimeHLine,
        "(iii)"
    },
    {
        "vline",
     	wrap_w4_runtimeVLine,
        "(iii)"
    },
    {
        "oval",
     	wrap_w4_runtimeOval,
        "(iiii)"
    },
    {
        "rect",
     	wrap_w4_runtimeRect,
        "(iiii)"
    },
    {
        "text",
     	wrap_w4_runtimeText,
        "(*ii)"
    },

    {
        "textUtf8",
     	wrap_w4_runtimeTextUtf8,
        "(iiii)"
    },
    {
        "textUtf16",
     	wrap_w4_runtimeTextUtf16,
        "(iiii)"
    },

    {
        "tone",
     	wrap_w4_runtimeTone,
        "(iiii)"
    },

    {
        "diskr",
     	wrap_w4_runtimeDiskr,
        "(ii)i"
    },
    {
        "diskw",
     	wrap_w4_runtimeDiskw,
        "(ii)i"
    },

    {
        "trace",
     	wrap_w4_runtimeTrace,
        "(i)"
    },

    {
        "traceUtf8",
     	wrap_w4_runtimeTraceUtf8,
        "(ii)"
    },
    {
        "traceUtf16",
     	wrap_w4_runtimeTraceUtf16,
        "(ii)"
    },
    {
        "tracef",
     	wrap_w4_runtimeTracef,
        "(ii)"
    },
};

extern unsigned char __game_card[];
extern unsigned int __game_card_len;
extern void *wamr_prepare_linear_memory(size_t size);
extern void wamr_cancel_prepared_linear_memory(void);

wasm_module_t wasm_module = NULL;
wasm_module_inst_t wasm_module_inst = NULL;
wasm_function_inst_t start = NULL;
wasm_function_inst_t update = NULL;
wasm_exec_env_t exec_env = NULL;
wasm_exec_env_t exec_env2 = NULL;

extern void run_wasm4(void *pvParameters);

void load_tinypong() {
    char error_buf[128];

    printf("WAMR instantiate config: stack=8192 heap=0\n");
    if (!wamr_prepare_linear_memory(64 * 1024)) {
        printf("Failed to reserve linear memory before load\n");
        return;
    }

    wasm_module = wasm_runtime_load(__game_card, __game_card_len, error_buf, sizeof(error_buf));
    if (!wasm_module) {
        printf("Failed to load wasm module: %s\n", error_buf);
        wamr_cancel_prepared_linear_memory();
        return;
    }

    printf("Instantiate the wasm module\n");
    /* The card starts with one 64-KiB WASM page.  WAMR rounds any non-zero
       host heap up to another full WASM page, so a 32-KiB heap would request
       128 KiB at once and exceed the ESP32-C5's largest free block (131072
       bytes) after allocator metadata.  The card has no exported malloc/free,
       so disable the optional host app heap for this game. */
    wasm_module_inst = wasm_runtime_instantiate(wasm_module, 8 * 1024, 0, error_buf, sizeof(error_buf));
    if (!wasm_module_inst) {
        printf("Failed to instantiate wasm module: %s\n", error_buf);
        wamr_cancel_prepared_linear_memory();
        wasm_runtime_unload(wasm_module);
        return;
    }

    start = wasm_runtime_lookup_function(wasm_module_inst, "start");
    printf("start: %p\n", start);

    /* moon.pkg.json exports the MoonBit `upd` function as `update`. */
    update = wasm_runtime_lookup_function(wasm_module_inst, "update");
    printf("update: %p\n", update);


    exec_env = wasm_runtime_create_exec_env(wasm_module_inst, 2 * 1024);
}

void init_wamr() {
    /* Setup variables for instantiating and running the wasm module */

    char error_buf[128];
    RuntimeInitArgs init_args;

    /* Configure memory allocation */
    memset(&init_args, 0, sizeof(RuntimeInitArgs));
    init_args.mem_alloc_type = Alloc_With_Allocator;
    init_args.mem_alloc_option.allocator.malloc_func = (void *)os_malloc;
    init_args.mem_alloc_option.allocator.realloc_func = (void *)os_realloc;
    init_args.mem_alloc_option.allocator.free_func = (void *)os_free;

    init_args.native_module_name = "env";
    init_args.native_symbols = native_symbols;
    init_args.n_native_symbols = sizeof(native_symbols) / sizeof(NativeSymbol);

    heap_caps_print_heap_info(MALLOC_CAP_8BIT);

    printf("Initialize WASM runtime\n");
    /* Initialize runtime environment */
    if (!wasm_runtime_full_init(&init_args)) {
        printf("Init runtime failed.\n");
        return;
    }

    printf("Load the wasm module from memory\n");
    printf("tinywasm: %p, len: %d\n", __game_card, __game_card_len);
    int count = 0;
    printf("count: %d\n", count);
    printf("memory test done\n");
    heap_caps_print_heap_info(MALLOC_CAP_8BIT);

    load_tinypong();

    /* 原示例在此进入 main.c 的游戏循环;我们的游戏任务由 cli 的 game 命令创建 */
    // run_wasm4(NULL);

    /* Clean up */
    // wasm_runtime_deinstantiate(wasm_module_inst);
    // wasm_runtime_unload(wasm_module);
    // wasm_runtime_destroy();
    // printf("WASM runtime destroyed.\n");
}

void* wamr_get_phy_memory() {
    if (!wasm_module_inst) {
        return NULL;
    }
    return wasm_runtime_addr_app_to_native(wasm_module_inst, 0);
}

void w4_wasmCallStart () {
    if (start) {
        printf("Call start %p\n", start);
        wasm_runtime_call_wasm(exec_env, start, 0, NULL);
    }
}

void w4_wasmCallUpdate () {
    static bool update_logged = false;
    update = wasm_runtime_lookup_function(wasm_module_inst, "update");
    if (!exec_env2) {
        exec_env2 = wasm_runtime_create_exec_env(wasm_module_inst, 10 * 1024);
    }
    if (!update_logged) {
        printf("Call update %p\n", update);
        update_logged = true;
    }
    if (!wasm_runtime_call_wasm(exec_env2, update, 0, NULL)) {
        const char *exception = wasm_runtime_get_exception(wasm_module_inst);
        printf("WASM update failed: %s\n",
               exception ? exception : "unknown exception");
    }
}
