#include "cli.h"
#include "esp_console.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "led.h"
#include "lcd.h"
#include "wifi_app.h"
#include "http_app.h"
#include "game_app.h"
#include <stdlib.h>

static const char *TAG = "cli";
uint16_t test1=0;

/* ============ NVS 助手:让灯记住自己的状态 ============ */

static void led_state_save(bool on)
{
    nvs_handle_t h;
    if (nvs_open("led_demo", NVS_READWRITE, &h) != ESP_OK) {
        return;
    }
    nvs_set_u8(h, "led_on", on ? 1 : 0);
    nvs_commit(h);      /* 关键:RAM 草稿 → 写进 Flash,断电才不丢 */
    nvs_close(h);
}

/* ============ 命令(人机适配层:解析参数 + 调用业务组件) ============ */

static int cmd_on(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    led_set(true);
    led_state_save(true);
    ESP_LOGI(TAG, "IO1/IO2 -> HIGH");

    return 0;
}

static int cmd_off(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    led_set(false);
    led_state_save(false);
    ESP_LOGI(TAG, "IO1/IO2 -> LOW");

    return 0;
}

static int cmd_hello(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("hello! I'm MZ56. type 'help' to see what I can do\n");
    return 0;
}

static int cmd_restart(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    printf("reboot now...\n");
    esp_restart();
    return 0;
}

static int cmd_scan(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    return wifi_app_scan();
}

static int cmd_join(int argc, char **argv)
{
    if (argc < 3) {
        printf("usage: join <ssid> <password>\n");
        return -1;
    }
    wifi_app_join(argv[1], argv[2]);
    return 0;
}

static int cmd_fetch(int argc, char **argv)
{
    if (argc < 2) {
        printf("usage: fetch <url>\n");
        return -1;
    }
    static char body[512];
    int n = http_app_get(argv[1], body, sizeof(body));
    if (n <= 0) {
        printf("抓取失败\n");
        return -1;
    }
    printf("--- 拿回 %d 字节 ---\n%s\n--- 结束 ---\n", n, body);
    return 0;
}

static int cmd_lcd(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (!lcd_init()) {
        printf("LCD 初始化失败\n");
        return -1;
    }

    lcd_clear(0x0000);   /* 先擦全屏:不给"旧画面留影"机会 */

    /* 编号色条卡:上白条/下蓝条(定上下),中间竖条纹 红绿蓝白循环(定左右顺序) */
    static const uint16_t stripe[4] = { 0xF800, 0x07E0, 0x001F, 0xFFFF };
    uint16_t *img = malloc(160 * 160 * 2);
    if (!img) {
        printf("内存不足\n");
        return -1;
    }
    /* 编号色块阵 4x4(每格 38px+2px 黑边):16 色各异,报位置=唯一解码 */
    static const uint16_t cells[16] = {
        0x0000, 0xF800, 0x07E0, 0x001F,   /* 黑 红 绿 蓝 */
        0xFFFF, 0xFFE0, 0x07FF, 0xF81F,   /* 白 黄 青 紫 */
        0x8000, 0x0400, 0x0010, 0x8410,   /* 暗红 暗绿 暗蓝 灰 */
        0xFBE0, 0x07E6, 0x86FF, 0xFBE7,   /* 橙 淡绿 淡紫 淡棕 */
    };
    for (int y = 0; y < 160; y++) {
        for (int x = 0; x < 160; x++) {
            int cx = x / 40, cy = y / 40;                 /* 所在格 */
            uint16_t c = cells[cy * 4 + cx];
            if ((x % 40) < 2 || (y % 40) < 2) c = 0xFFFF; /* 白色格线 */
            img[y * 160 + x] = c;
        }
    }

    /* 面板实测会转 90°:送图前先旋转回去。LCD_ROT 换个方向试试:
     * 1 = 顺时针转 90°(逆补),改 0 = 逆时针 90°(若反向) */
#define LCD_ROT  1
    uint16_t *rot = malloc(160 * 160 * 2);
    if (!rot) {
        free(img);
        printf("内存不足\n");
        return -1;
    }
    for (int y = 0; y < 160; y++) {
        for (int x = 0; x < 160; x++) {
#if LCD_ROT
            rot[x * 160 + (159 - y)] = img[y * 160 + x];   /* 顺时针 90° 逆映射 */
#else
            rot[(159 - x) * 160 + y] = img[y * 160 + x];   /* 逆时针 90° */
#endif
        }
    }
    lcd_draw_img(68, 40, 160, 160, rot);   /* 居中贴到 296x240 */
    free(rot);
    free(img);
    printf("LCD 测试卡完成: 已做 90° 校正上屏\n");
    return 0;
}

static int cmd_game(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    if (!lcd_init()) {          /* 先确保屏就位(幂等,已初始化则直接通过) */
        printf("LCD 未就绪\n");
        return -1;
    }
    game_app_start();           /* 游戏任务接管;屏幕看效果,按键阶段 2 */
    printf("wasm4 启动中(屏幕看效果,按键阶段 2)\n");
    return 0;
}

static int cmd_time(int argc, char **argv)
{
    (void)argc;
    (void)argv;
    static char body[512];
    int n = http_app_get("http://worldtimeapi.org/api/ip", body, sizeof(body));
    if (n <= 0) {
        printf("时间获取失败(检查网络/连接)\n");
        return -1;
    }
    /* 轻量"抓关键词":JSON 里找 "datetime" 字段的值 */
    const char *p = strstr(body, "\"datetime\"");
    if (p) {
        printf("世界时间: %s\n", p + 12);   /* 跳过 "datetime": 这 11 个字符 */
    } else {
        printf("%s\n", body);               /* 没找到就原样吐出来 */
    }
    return 0;
}

/* ============ 启动 ============ */

void cli_init(void)
{
    /* ---------- NVS 初始化 + 恢复上次状态 ---------- */
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        /* 分区损坏/升级:格式化重来 */
        ESP_ERROR_CHECK(nvs_flash_erase());
        ESP_ERROR_CHECK(nvs_flash_init());
    }

    nvs_handle_t h;
    if (nvs_open("led_demo", NVS_READONLY, &h) == ESP_OK) {
        uint8_t v = 0;
        if (nvs_get_u8(h, "led_on", &v) == ESP_OK && v != 0) {
            led_set(true);
            ESP_LOGI(TAG, "restored: LED was ON");
        }
        nvs_close(h);
    }

    /* ---------- 创建 REPL(挂在 USB-Serial-JTAG) ---------- */
    esp_console_repl_t *repl = NULL;
    esp_console_repl_config_t repl_cfg = ESP_CONSOLE_REPL_CONFIG_DEFAULT();
    repl_cfg.prompt = "led> ";

    esp_console_dev_usb_serial_jtag_config_t usb_cfg = ESP_CONSOLE_DEV_USB_SERIAL_JTAG_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_console_new_repl_usb_serial_jtag(&usb_cfg, &repl_cfg, &repl));

    /* ---------- 登记命令 ---------- */
    static const esp_console_cmd_t cmd_tab[] = {
        { .command = "on",      .help = "IO1/IO2 -> HIGH",    .func = cmd_on },
        { .command = "off",     .help = "IO1/IO2 -> LOW",     .func = cmd_off },
        { .command = "hello",   .help = "say hello",          .func = cmd_hello },
        { .command = "restart", .help = "reboot the device",  .func = cmd_restart },
        { .command = "scan",    .help = "wifi APs near by",   .func = cmd_scan },
        { .command = "join",    .help = "join <ssid> <password>", .func = cmd_join },
        { .command = "fetch",   .help = "fetch <url> (http get)", .func = cmd_fetch },
        { .command = "time",    .help = "world time from internet", .func = cmd_time },
        { .command = "lcd",     .help = "show 160x160 gradient on LCD", .func = cmd_lcd },
        { .command = "game",    .help = "boot wasm4 cart (snake)",       .func = cmd_game },
    };
    for (int i = 0; i < sizeof(cmd_tab) / sizeof(cmd_tab[0]); i++) {
        ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_tab[i]));
    }

    /* ---------- 开业 ---------- */
    ESP_LOGI(TAG, "console started, type 'help'");
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
