#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "led.h"

static const char *TAG_IO1 = "io1";
static const char *TAG_IO2 = "io2";

/* 任务 A:IO1 每 1 秒翻转一次 */
static void io1_task(void *arg)
{
    while (1) {
        led_set_pin(LED_GPIO_1, true);
        ESP_LOGI(TAG_IO1, "IO1 HIGH");
        vTaskDelay(pdMS_TO_TICKS(1000));   /* 睡眠 1000ms = 100 tick,CPU 让给别人 */
        led_set_pin(LED_GPIO_1, false);
        ESP_LOGI(TAG_IO1, "IO1 LOW");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* 任务 B:IO2 每 330ms 翻转一次(333ms 会被 tick 量化成 33 个 tick = 330ms) */
static void io2_task(void *arg)
{
    while (1) {
        led_set_pin(LED_GPIO_2, true);
        ESP_LOGI(TAG_IO2, "IO2 HIGH");
        vTaskDelay(pdMS_TO_TICKS(333));
        led_set_pin(LED_GPIO_2, false);
        ESP_LOGI(TAG_IO2, "IO2 LOW");
        vTaskDelay(pdMS_TO_TICKS(333));
    }
}

void app_main(void)
{
    led_init();
    ESP_LOGI(TAG_IO1, "two tasks start: io1 1s / io2 330ms");

    /* xTaskCreate(任务函数, 任务名, 栈大小[字节], 参数, 优先级, 句柄) */
    xTaskCreate(io1_task, "io1_task", 2048, NULL, 5, NULL);
    xTaskCreate(io2_task, "io2_task", 2048, NULL, 5, NULL);

    vTaskDelete(NULL);   /* app_main 功成身退:创建完任务,把自己删了 */
}
