#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "led.h"
#include "cli.h"

static const char *TAG = "main";
extern uint16_t test1;
void app_main(void)
{
    led_init();
    ESP_LOGI(TAG, "app start: BSP(LED) + CLI(console)");

    cli_init();   /* 内部启动 REPL,永不返回 */

    while (1) {   /* 兜底:app_main 永远不许返回 */
        vTaskDelay(pdMS_TO_TICKS(60000));
    }
}
