#include "wifi_app.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_netif.h"
#include "esp_log.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *TAG = "wifi_app";
static bool s_wifi_started = false;
static bool s_join_requested = false;   /* 意图标志:只有 join 下令后才允许自动重连 */

/* 事件回调:WIFI/网络事件都从这里被"广播"给系统 */
static void wifi_evt(void *arg, esp_event_base_t base, int32_t id, void *data)
{
    (void)arg;
    (void)base;
    if (base == WIFI_EVENT && id == WIFI_EVENT_STA_START) {
        ESP_LOGI(TAG, "sta start (wait for command)");   /* 只登记,不自动连接! */
    } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_CONNECTED) {
        ESP_LOGI(TAG, "connected to AP, waiting for IP...");
    } else if (base == WIFI_EVENT && id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_join_requested) {           /* 意图门:你 join 过,才帮你重连 */
            ESP_LOGW(TAG, "connection lost, retrying...");
            esp_wifi_connect();
        }
    } else if (base == IP_EVENT && id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t *evt = (ip_event_got_ip_t *)data;
        ESP_LOGI(TAG, "GOT_IP: " IPSTR, IP2STR(&evt->ip_info.ip));
    }
}

bool wifi_app_init_once(void)
{
    if (s_wifi_started) {
        return true;
    }
    /* 降噪:系统级 wifi 日志提到 ERROR(否则连接过程 I/W 刷屏);
     * 我们自己的 TAG("wifi_app")不受影响,航标照常 */
    esp_log_level_set("wifi", ESP_LOG_ERROR);
    esp_log_level_set("wifi_init", ESP_LOG_WARN);
    esp_log_level_set("pp", ESP_LOG_WARN);
    esp_log_level_set("net80211", ESP_LOG_WARN);

    ESP_LOGI(TAG, "init wifi engine...");
    esp_netif_init();
    esp_event_loop_create_default();
    /* 创建 STA 网络接口实例:DHCP 客户端就住在它身上,
     * 有了它才有 IP 认证流程(GOT_IP 由它颁发)! */
    esp_netif_create_default_wifi_sta();
    esp_event_handler_instance_register(WIFI_EVENT, ESP_EVENT_ANY_ID, wifi_evt, NULL, NULL);
    esp_event_handler_instance_register(IP_EVENT, ESP_EVENT_ANY_ID, wifi_evt, NULL, NULL);
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    esp_wifi_init(&cfg);
    esp_wifi_set_mode(WIFI_MODE_STA);
    esp_wifi_start();
    s_wifi_started = true;
    return true;
}

int wifi_app_scan(void)
{
    wifi_app_init_once();

    wifi_scan_config_t sconf = {
        .ssid = NULL, .bssid = NULL, .channel = 0, .scan_type = WIFI_SCAN_TYPE_ACTIVE,
    };
    if (esp_wifi_scan_start(&sconf, true) != ESP_OK) {
        return -1;
    }

    uint16_t count = 0;
    esp_wifi_scan_get_ap_num(&count);
    printf("found %d APs nearby:\n", count);

    wifi_ap_record_t *recs = calloc(count ? count : 1, sizeof(*recs));
    if (!recs) {
        return -1;
    }
    esp_wifi_scan_get_ap_records(&count, recs);
    for (uint16_t i = 0; i < count; i++) {
        printf("  [%2d] RSSI %4d dBm   %-32s\n",
               i, recs[i].rssi, (const char *)recs[i].ssid);
    }
    free(recs);
    return 0;
}

bool wifi_app_join(const char *ssid, const char *password)
{
    wifi_app_init_once();

    wifi_config_t cfg = { 0 };
    strncpy((char *)cfg.sta.ssid, ssid, sizeof(cfg.sta.ssid) - 1);
    if (password != NULL) {
        strncpy((char *)cfg.sta.password, password, sizeof(cfg.sta.password) - 1);
    }
    esp_wifi_disconnect();                /* 先打断旧的连接尝试("正在发车"则先下车) */
    esp_wifi_set_config(WIFI_IF_STA, &cfg);

    s_join_requested = true;              /* 放下"意图牌":现在可以自动重连了 */
    ESP_LOGI(TAG, "connecting to \"%s\" ...", ssid);
    esp_wifi_connect();
    return true;
}
