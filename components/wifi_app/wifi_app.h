#ifndef WIFI_APP_H
#define WIFI_APP_H

#include <stdbool.h>

/* 懒初始化 WiFi 引擎(驱动+事件回调),内部保证只做一次 */
bool wifi_app_init_once(void);

/* 扫描周围 AP,打印"名字/信号强度"表 */
int wifi_app_scan(void);

/* 连接指定 WiFi;成功后由事件回调打印 GOT_IP */
bool wifi_app_join(const char *ssid, const char *password);

#endif /* WIFI_APP_H */
