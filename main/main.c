#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_system.h"
#include "led.h"
#include "cli.h"
#include "lcd.h"
#include "game_app.h"

static const char *TAG = "main";
extern uint16_t test1;
void app_main(void)
{
    printf("reset reason=%d\n", (int)esp_reset_reason());
    led_init();
    ESP_LOGI(TAG, "app start: BSP(LED) + CLI(console)");

    /* The device keeps the game alive independently of the USB console. */
    if (lcd_init())
        game_app_start();
    else
        ESP_LOGE(TAG, "LCD initialization failed; game not started");

    cli_init();   /* 内部启动 REPL,永不返回 */

    while (1) {   /* 兜底:app_main 永远不许返回 */
        vTaskDelay(pdMS_TO_TICKS(60000));
    }
}
