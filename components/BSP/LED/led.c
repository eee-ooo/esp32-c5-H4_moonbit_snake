#include "led.h"
#include "driver/gpio.h"

/* 位掩码:1ULL << n 表示第 n 个引脚;64 位长,覆盖 IO0~IO63 */
#define LED_PIN_MASK  (1ULL << LED_GPIO)

void led_init(void)
{
    gpio_config_t io = {
        .pin_bit_mask = LED_PIN_MASK,        /* 选中演示引脚 */
        /* 输出 + 输入:二者都要开。纯 GPIO_MODE_OUTPUT 会关闭输入缓冲,
         * 导致 gpio_get_level() 恒为 0(回读功能失效)。 */
        .mode         = GPIO_MODE_INPUT_OUTPUT,
        .pull_up_en   = GPIO_PULLUP_DISABLE, /* 输出模式用不上内部上拉 */
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,   /* 输出脚不需要中断 */
    };
    gpio_config(&io);

    gpio_set_level(LED_GPIO, 0);   /* 初始电平拉低,避免上电状态不定 */
}

void led_set_pin(int pin, bool state)
{
    gpio_set_level(pin, state ? 1 : 0);
}

void led_set(bool state)
{
    led_set_pin(LED_GPIO, state);
}

bool led_get(void)
{
    return gpio_get_level(LED_GPIO) != 0;
}

bool led_get_pin(int pin)
{
    return gpio_get_level(pin) != 0;
}

/* 禁止被"探针"操作的引脚:
 *   0~4, 9 → LCD(MOSI/SCLK/DC/RST/CS/背光)
 *   27     → 电源保持(拉低可能断电)
 *   28     → BOOT 按键 / 启动配置脚 */
static bool pin_forbidden(int pin)
{
    switch (pin) {
    case 0: case 1: case 2: case 3: case 4: case 9:
    case 27: case 28:
        return true;
    default:
        return false;
    }
}

bool led_probe(int pin, bool state)
{
    if (pin < 0 || pin > 28 || pin_forbidden(pin)) {
        return false;
    }

    gpio_config_t io = {
        .pin_bit_mask = 1ULL << pin,
        .mode         = GPIO_MODE_INPUT_OUTPUT,   /* 同 led_init:回读需要输入缓冲 */
        .pull_up_en   = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,
    };
    if (gpio_config(&io) != ESP_OK) {
        return false;
    }
    gpio_set_level(pin, state ? 1 : 0);
    return true;
}
