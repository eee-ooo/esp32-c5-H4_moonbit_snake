#ifndef LED_H
#define LED_H

#include <stdbool.h>

/* MZ56-ESP32-C5 板载无 LED,这里用 IO1 / IO2 做 GPIO 电平翻转演示 */
#define LED_GPIO_1   1
#define LED_GPIO_2   2

/* 初始化 IO1/IO2 为推挽输出,默认输出低电平 */
void led_init(void);

/* 设置两个 IO 的输出电平:state=true 输出高(3.3V),false 输出低(0V) */
void led_set(bool state);

#endif /* LED_H */
