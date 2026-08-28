#include "cli.h"
#include "esp_console.h"
#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "led.h"

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

/* ============ 命令 ============ */

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
    };
    for (int i = 0; i < sizeof(cmd_tab) / sizeof(cmd_tab[0]); i++) {
        ESP_ERROR_CHECK(esp_console_cmd_register(&cmd_tab[i]));
    }

    /* ---------- 开业 ---------- */
    ESP_LOGI(TAG, "console started, type 'help'");
    ESP_ERROR_CHECK(esp_console_start_repl(repl));
}
