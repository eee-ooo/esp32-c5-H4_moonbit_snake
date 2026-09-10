#include <stdio.h>
#include "runtime.h"
#include "window.h"
#include "control.h"
#include "wasm_export.h"
#include "esp_log.h"
#include "game_app.h"
#include "lcd.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "pthread.h"

static const char *TAG = "game";
static bool s_game_running;

extern wasm_module_t wasm_module;
extern wasm_module_inst_t wasm_module_inst;
extern wasm_function_inst_t start;
extern wasm_function_inst_t update;
extern wasm_exec_env_t exec_env;

static void *w4_windowBoot_pthread(void *arg);

static void *wamr_init_thread(void *arg)
{
    (void)arg;
    extern void init_wamr(void);
    init_wamr();
    return NULL;
}

/* 主循环:每帧 gamepad→update(w4_runtimeUpdate 会驱动 wasm 的 start/update) */
void w4_windowBoot(void)
{
    while (1) {
        if (!wasm_module_inst || !start || !update || !exec_env) {
            vTaskDelay(pdMS_TO_TICKS(50));
            continue;
        }
        w4_runtimeSetGamepad(0, get_player_state(0));
        w4_runtimeSetGamepad(1, get_player_state(1));
        clear_all_player_state();
        w4_runtimeUpdate();
        vTaskDelay(pdMS_TO_TICKS(16));   /* ~60fps */
    }
}

/* RGB888→RGB565 */
static uint16_t to565(uint32_t c)
{
    uint8_t r = (c >> 16) & 0xFF, g = (c >> 8) & 0xFF, b = c & 0xFF;
    return ((r >> 3) << 11) | ((g >> 2) << 5) | (b >> 3);
}

/* Stream the 160x160 game image in small blocks instead of retaining a 50KB
   RGB565 frame.  The ESP32-C5's DRAM is also needed by WAMR. */
#define W4_BLIT_ROWS 8
static uint16_t s_fb[2][160 * W4_BLIT_ROWS];

/* 合成:2bpp 索引帧 + 4 色 palette → RGB565 缓冲 → 整帧贴屏(居中 68,40) */
void w4_windowComposite(const uint32_t *palette, const uint8_t *framebuffer)
{
    for (int y = 0; y < 160; y += W4_BLIT_ROWS) {
        uint16_t *block = s_fb[(y / W4_BLIT_ROWS) & 1];
        int rows = 160 - y;
        if (rows > W4_BLIT_ROWS)
            rows = W4_BLIT_ROWS;

        for (int row = 0; row < rows; row++) {
            int base = (y + row) * 160;
            for (int x = 0; x < 160; x++) {
                int i = base + x;
                uint8_t px = framebuffer[i >> 2];
                int sel = (px >> (2 * (i & 3))) & 3;
                block[row * 160 + x] = to565(palette[sel]);
            }
        }

        lcd_draw_img(68, 40 + y, 160, rows, block);
    }
}

/* Initialize WAMR and start the independent game thread. */
void game_app_start(void)
{
    if (s_game_running) {
        ESP_LOGI(TAG, "game already running");
        return;
    }

    ESP_LOGI(TAG, "booting wasm4...");

    init_button();   /* 配置按键 GPIO:必须在游戏线程开始读取之前完成 */

    pthread_t init_thread;
    pthread_attr_t attr;
    int rc;

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);
    pthread_attr_setstacksize(&attr, 5 * 1024);
    rc = pthread_create(&init_thread, &attr, wamr_init_thread, NULL);
    pthread_attr_destroy(&attr);
    if (rc != 0) {
        ESP_LOGE(TAG, "WAMR init thread create failed: %d", rc);
        return;
    }
    pthread_join(init_thread, NULL);

    if (!wasm_module_inst) {
        ESP_LOGE(TAG, "WASM initialization failed; game task not started");
        return;
    }

    pthread_attr_init(&attr);
    pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
    pthread_attr_setstacksize(&attr, 8 * 1024);
    rc = pthread_create(&init_thread, &attr, w4_windowBoot_pthread, NULL);
    pthread_attr_destroy(&attr);
    if (rc != 0) {
        ESP_LOGE(TAG, "WASM game thread create failed: %d", rc);
        return;
    }
    s_game_running = true;
}

static void *w4_windowBoot_pthread(void *arg)
{
    (void)arg;
    extern void *wamr_get_phy_memory(void);
    w4_Disk disk = { 0 };
    uint8_t *mem = wamr_get_phy_memory();
    if (!mem) {
        ESP_LOGE(TAG, "WASM instance unavailable; stopping game task");
        return NULL;
    }
    w4_runtimeInit(mem, &disk);
    w4_windowBoot();
    return NULL;
}
