#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "led.h"

static const char *TAG = "main";
uint16_t test=0;
void app_main(void)
{
    led_init();
    ESP_LOGI(TAG, "IO1/IO2 toggle demo start, 1s per flip");

    while (1) {
        led_set(true);
        ESP_LOGI(TAG, "IO1/IO2 -> HIGH (3.3V)");
        vTaskDelay(pdMS_TO_TICKS(1000));   /* 睡 1 秒,期间 CPU 可以干别的 */
        test=1;
        led_set(false);
        ESP_LOGI(TAG, "IO1/IO2 -> LOW (0V)");
        vTaskDelay(pdMS_TO_TICKS(1000));
        test=0;
    }
}
