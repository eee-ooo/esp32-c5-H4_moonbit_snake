#include "lcd.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_st7789.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

static const char *TAG = "lcd";

/* 厂商接线图(DOIT_AI doit-ai-c5-kit-lcd) */
#define LCD_SCLK_GPIO  1
#define LCD_MOSI_GPIO  0
#define LCD_CS_GPIO    4
#define LCD_DC_GPIO    2
#define LCD_RST_GPIO   3
#define LCD_BL_GPIO    9

/* 背光极性:0=高平电亮(已实测 OK);厂商的"INVERT"是它 PWM 驱动的极性,与 GPIO 无关 */
#define LCD_BL_INVERT  0

static esp_lcd_panel_handle_t s_panel = NULL;
static bool s_lcd_ready = false;   /* 幂等守卫:重复初始化直接返回 */

static void lcd_backlight(bool on)
{
    gpio_config_t io = {
        .pin_bit_mask = 1ULL << LCD_BL_GPIO,
        .mode = GPIO_MODE_OUTPUT,
    };
    gpio_config(&io);
    gpio_set_level(LCD_BL_GPIO, (on ^ LCD_BL_INVERT) ? 1 : 0);
}

bool lcd_init(void)
{
    if (s_lcd_ready) {
        return true;   /* 已经初始化过,直接说成功 */
    }
    /* 1. SPI 总线 —— 用回"已验证能跑"的配置(0=不配 DMA、50KB 预算) */
    spi_bus_config_t buscfg = {
        .mosi_io_num = LCD_MOSI_GPIO,
        .sclk_io_num = LCD_SCLK_GPIO,
        .quadwp_io_num = -1, .quadhd_io_num = -1,
        .max_transfer_sz = LCD_WIDTH * 80 * 2,
    };
    if (spi_bus_initialize(SPI2_HOST, &buscfg, 0) != ESP_OK) {
        ESP_LOGE(TAG, "spi bus init failed");
        return false;
    }

    /* 2. LCD 面板 IO —— 照抄厂商:spi_mode=3, 80MHz, 队列深 5 */
    esp_lcd_panel_io_spi_config_t io_config = {
        .cs_gpio_num = LCD_CS_GPIO,
        .dc_gpio_num = LCD_DC_GPIO,
        .spi_mode = 3,
        .pclk_hz = 80 * 1000 * 1000,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        /* lcd_draw_img reuses its small streaming buffer immediately; keep
           one transaction in flight so the buffer is not overwritten while
           SPI DMA is still reading it. */
        .trans_queue_depth = 1,
    };
    esp_lcd_panel_io_handle_t io_handle = NULL;
    if (esp_lcd_new_panel_io_spi(SPI2_HOST, &io_config, &io_handle) != ESP_OK) {
        ESP_LOGE(TAG, "panel io failed");
        return false;
    }

    /* 3. ST7789 面板 —— 厂商:RGB 元素序(不是 BGR!),复位两遍+100ms 等待 */
    esp_lcd_panel_dev_config_t panel_cfg = {
        .reset_gpio_num = LCD_RST_GPIO,
        .rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB,
        .bits_per_pixel = 16,
    };
    if (esp_lcd_new_panel_st7789(io_handle, &panel_cfg, &s_panel) != ESP_OK) {
        ESP_LOGE(TAG, "panel init failed");
        return false;
    }

    esp_lcd_panel_reset(s_panel);
    vTaskDelay(pdMS_TO_TICKS(100));
    esp_lcd_panel_reset(s_panel);

    esp_lcd_panel_init(s_panel);
    esp_lcd_panel_invert_color(s_panel, true);          /* 厂商: true */
    esp_lcd_panel_swap_xy(s_panel, true);               /* 回厂商: true */
    esp_lcd_panel_mirror(s_panel, true, false);         /* 回厂商: MIRROR_X=true */
    esp_lcd_panel_set_gap(s_panel, 24, 0);              /* 2.0" 屏经典窗口偏移 */
    esp_lcd_panel_disp_on_off(s_panel, true);

    lcd_backlight(true);
    s_lcd_ready = true;
    ESP_LOGI(TAG, "backlight on (BL=%d, invert=%d)", LCD_BL_GPIO, LCD_BL_INVERT);
    return true;
}

void lcd_clear(uint16_t color)
{
    if (!s_panel) {
        return;
    }
    /* With MV (swap_xy) enabled, the 24-pixel x gap is visible as an
       uncovered strip on the other axis. Clear the raw 320x240 GRAM window
       with no software gap, then restore the game's 24-pixel viewport gap. */
    const int clear_width = 320;
    const int clear_height = 240;
    static uint16_t row[320];
    esp_lcd_panel_set_gap(s_panel, 0, 0);
    for (int i = 0; i < clear_width; i++) {
        row[i] = color;
    }
    for (int y = 0; y < clear_height; y++) {
        esp_lcd_panel_draw_bitmap(s_panel, 0, y, clear_width, y + 1, row);
        /* The row buffer is reused on the next iteration. Give the SPI DMA
           transaction time to finish before overwriting it. */
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    esp_lcd_panel_set_gap(s_panel, 24, 0);
}

void lcd_draw_img(int x, int y, int w, int h, const uint16_t *img)
{
    if (!s_panel) {
        return;
    }
    esp_lcd_panel_draw_bitmap(s_panel, x, y, x + w, y + h, img);
}
