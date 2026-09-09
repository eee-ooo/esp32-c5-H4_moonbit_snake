#ifndef LCD_H
#define LCD_H

#include <stdint.h>
#include <stdbool.h>

/* MZ56-ESP32-C5 配套 2.0" ST7789 屏(接线/参数取自 DOIT_AI doit-ai-c5-kit-lcd) */
#define LCD_WIDTH  296   /* 商家逻辑宽:320 物理窗 - 24 偏移 = 296 */
#define LCD_HEIGHT 240

/* 初始化屏幕(初始化 SPI 总线 + ST7789 面板 + 背光) */
bool lcd_init(void);

/* 清屏:全屏填一个颜色(RGB565) */
void lcd_clear(uint16_t color);

/* 把一块 RGB565 图贴到逻辑坐标 (x,y),宽高 w×h */
void lcd_draw_img(int x, int y, int w, int h, const uint16_t *img);

#endif /* LCD_H */
