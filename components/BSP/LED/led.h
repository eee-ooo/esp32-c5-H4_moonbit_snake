#ifndef LED_H
#define LED_H

#include <stdbool.h>

/* MZ56-ESP32-C5 板载无 LED:用一个"空闲 GPIO"做电平输出演示。
 *
 * 选脚依据(本板占用情况):
 *   LCD   : IO0 / IO1 / IO2 / IO3 / IO4 / IO9
 *   UART0 : IO11(TX0) / IO12(RX0)        —— 排针上标 TX0 / RX0
 *   保留  : IO27(电源保持) / IO28(BOOT 按键、启动配置脚)
 *   USB   : IO13 / IO14 可能为 USB D±,不建议占用
 * 故默认选 IO26(排针已引出,且未见于以上用途)。
 * 若该脚在板上另有他用,只需改下面一行(候选:IO5 / IO7 / IO8 / IO10)。 */
#define LED_GPIO   26

/* 初始化为推挽输出,默认输出低电平 */
void led_init(void);

/* 设置输出电平:state=true 输出高(3.3V),false 输出低(0V) */
void led_set(bool state);

/* 回读该引脚的实际电平(用于验证引脚确实受本模块控制) */
bool led_get(void);

/* 回读任意引脚的实际电平 */
bool led_get_pin(int pin);

/* 排查空闲引脚用:把任意引脚配成输出并设置电平。
 * 返回 false 表示该引脚在禁止名单内(LCD 占用 / 电源保持 / BOOT),
 * 避免误操作导致花屏或断电。 */
bool led_probe(int pin, bool state);

/* 控制任意引脚(引脚号传入),供多任务各自管理引脚时使用 */
void led_set_pin(int pin, bool state);

#endif /* LED_H */
