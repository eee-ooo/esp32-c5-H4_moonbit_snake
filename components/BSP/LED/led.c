#include "led.h"
#include "driver/gpio.h"

/* 位掩码:1ULL << n 表示第 n 个引脚;64 位长,覆盖 IO0~IO63 */
#define LED_PIN_MASK  ((1ULL << LED_GPIO_1) | (1ULL << LED_GPIO_2))

void led_init(void)
{
    gpio_config_t io = {
        .pin_bit_mask = LED_PIN_MASK,        /* 选中 IO1 和 IO2 两个脚 */
        .mode         = GPIO_MODE_OUTPUT,    /* 推挽输出 */
        .pull_up_en   = GPIO_PULLUP_DISABLE, /* 输出模式用不上内部上拉 */
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,   /* 输出脚不需要中断(以后按键才用) */
    };
    gpio_config(&io);     /* 一次调用,配置全部生效 */

    gpio_set_level(LED_GPIO_1, 0);  /* 初始电平拉低,避免上电状态不定 */
    gpio_set_level(LED_GPIO_2, 0);
}

void led_set_pin(int pin, bool state)
{
    gpio_set_level(pin, state ? 1 : 0);
}

void led_set(bool state)
{
    led_set_pin(LED_GPIO_1, state);
    led_set_pin(LED_GPIO_2, state);
}
