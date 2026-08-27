#ifndef LED_H
#define LED_H

#include <stdbool.h>

/* MZ56-ESP32-C5 板载无 LED,这里用 IO1 / IO2 做 GPIO 电平翻转演示 */
#define LED_GPIO_1   1
#define LED_GPIO_2   2

/* 初始化 IO1/IO2 为推挽输出,默认输出低电平 */
void led_init(void);

/* 设置所有 LED 的输出电平:state=true 输出高(3.3V),false 输出低(0V) */
void led_set(bool state);

/* 只控制某一个引脚(引脚号用 LED_GPIO_x 宏):多任务各自管理引脚时使用 */
void led_set_pin(int pin, bool state);

#endif /* LED_H */
