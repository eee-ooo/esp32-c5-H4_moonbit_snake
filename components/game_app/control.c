#include "control.h"
#include "runtime.h"
#include "driver/gpio.h"
#include "esp_log.h"

/* 两个按键 → 相对转向:
 *     GPIO7 按下 = 左转 90°(逆时针)
 *     GPIO8 按下 = 右转 90°(顺时针)
 *
 * 接法:按键一端接 GPIO,另一端接 GND;芯片内部上拉,按下读到低电平。
 *
 * 为什么要"翻译"(而不是直接映射成左/右):
 *   游戏里的方向是**绝对方向**,而且带门禁——
 *     · 只有正在**横向**移动时,才接受 上/下
 *     · 只有正在**竖向**移动时,才接受 左/右
 *   初始方向是"向右",此时按"左"会被门禁挡掉,等于按了没用。
 *   因此这里维护一份"当前朝向"的影子(初始向右,与游戏一致),
 *   把"左转/右转"翻译成游戏能接受的绝对方向键。
 *   该游戏没有重开逻辑(方向永不复位),所以这份影子不会失同步。 */

#define BTN_TURN_LEFT_GPIO   7      /* 左转(逆时针) */
#define BTN_TURN_RIGHT_GPIO  8      /* 右转(顺时针) */

#define BTN_COUNT            2
#define BTN_STABLE_FRAMES    3      /* 连续 3 帧(约 50ms)电平一致才算数 */

static const char *TAG = "button";

static const int s_pins[BTN_COUNT] = { BTN_TURN_LEFT_GPIO, BTN_TURN_RIGHT_GPIO };
static uint8_t   s_last[BTN_COUNT] = { 1, 1 };   /* 上次采样电平(1=松开) */
static uint8_t   s_cnt[BTN_COUNT];               /* 连续一致帧数 */
static bool      s_fired[BTN_COUNT];             /* 本次按下是否已触发 */

static int s_dx = 1, s_dy = 0;                   /* 朝向影子:初始向右 */

uint8_t gamepads[4] = { 0 };

void init_button(void)
{
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << BTN_TURN_LEFT_GPIO) | (1ULL << BTN_TURN_RIGHT_GPIO),
        .mode         = GPIO_MODE_INPUT,
        .pull_up_en   = GPIO_PULLUP_ENABLE,     /* 内部上拉 */
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type    = GPIO_INTR_DISABLE,      /* 每帧轮询,不需要中断 */
    };
    gpio_config(&io);

    ESP_LOGI(TAG, "two buttons ready: GPIO%d = turn left, GPIO%d = turn right (active low)",
             BTN_TURN_LEFT_GPIO, BTN_TURN_RIGHT_GPIO);
}

/* 去抖后的"按下沿":按住不重复触发,松开后再按才再次触发 */
static bool pressed_once(int idx)
{
    int lvl = gpio_get_level(s_pins[idx]);

    if (lvl == s_last[idx]) {
        if (s_cnt[idx] < BTN_STABLE_FRAMES) {
            s_cnt[idx]++;
        }
    } else {
        s_last[idx] = (uint8_t)lvl;
        s_cnt[idx] = 0;
        s_fired[idx] = false;                   /* 电平变化 → 重新武装 */
    }

    if (lvl == 0 && s_cnt[idx] >= BTN_STABLE_FRAMES && !s_fired[idx]) {
        s_fired[idx] = true;
        return true;
    }
    return false;
}

/* 把当前朝向旋转 90°(屏幕坐标:y 向下),返回对应的 WASM-4 绝对方向键 */
static uint8_t turn(bool clockwise)
{
    int dx = clockwise ? -s_dy :  s_dy;         /* 顺时针:(x,y)→(-y, x) */
    int dy = clockwise ?  s_dx : -s_dx;         /* 逆时针:(x,y)→( y,-x) */

    s_dx = dx;
    s_dy = dy;

    if (dx > 0) return W4_BUTTON_RIGHT;
    if (dx < 0) return W4_BUTTON_LEFT;
    if (dy > 0) return W4_BUTTON_DOWN;
    return W4_BUTTON_UP;
}

void set_player_state(int player, uint8_t state)
{
    if (player >= 0 && player < 4) {
        gamepads[player] = state;
    }
}

/* 每帧被 display.c 调用一次:返回本帧要发给游戏的 gamepad 字节 */
uint8_t get_player_state(int player)
{
    if (player != 0) {
        return 0;                               /* 本工程只用玩家 1 */
    }

    /* 方向键只在"按下那一帧"发一个脉冲:游戏按"字节变化"触发,
     * 脉冲持续一帧正好让它看到一次变化,下一帧回到 0。 */
    if (pressed_once(0)) {
        return turn(false);                     /* 左键 = 逆时针 */
    }
    if (pressed_once(1)) {
        return turn(true);                      /* 右键 = 顺时针 */
    }
    return 0;
}

void clear_all_player_state(void)
{
    /* 状态由按下沿即时产生,无需清理 */
}
