#include "control.h"

/* 阶段 1:按键打桩 —— 厂商的按键引脚(2/3/10/11)与我们屏/串口冲突,
 * 先全部返回"未按下"。阶段 2 再为真按钮接真实 GPIO。 */

uint8_t gamepads[4] = { 0 };

void init_button(void) { }

void set_player_state(int player, uint8_t state)
{
    (void)player;
    (void)state;
}

uint8_t get_player_state(int player)
{
    (void)player;
    return 0;
}

void clear_all_player_state(void) { }
