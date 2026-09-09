#include "http_app.h"
#include "esp_http_client.h"
#include "esp_log.h"

static const char *TAG = "http_app";

int http_app_get(const char *url, char *buf, size_t buf_len)
{
    if (buf == NULL || buf_len == 0) {
        return -1;
    }

    /* ① 配置单:要访问的地址、方式、超时、允许跳几次 */
    esp_http_client_config_t cfg = {
        .url = url,
        .method = HTTP_METHOD_GET,
        .timeout_ms = 8000,
        .max_redirection_count = 3,   /* 跟随跳转最多 3 次 */
    };

    /* ② 创建会话(打开通往服务器的路) */
    esp_http_client_handle_t h = esp_http_client_init(&cfg);
    if (h == NULL) {
        ESP_LOGE(TAG, "会话创建失败");
        return -1;
    }

    /* ③ 一次走完:握手 → 发请求 → 收应答头 */
    esp_err_t err = esp_http_client_perform(h);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "请求失败: %s", esp_err_to_name(err));
        esp_http_client_cleanup(h);
        return -1;
    }

    /* ④ 看看服务器说了什么状态码(200 = 好) */
    int status = esp_http_client_get_status_code(h);
    ESP_LOGI(TAG, "服务器状态码: %d", status);
    if (status != 200) {
        esp_http_client_cleanup(h);
        return -1;
    }

    /* ⑤ 读取响应体(最多装进缓冲区) */
    int content_len = esp_http_client_get_content_length(h);
    if (content_len <= 0 || content_len > (int)buf_len - 1) {
        content_len = (int)buf_len - 1;
    }
    int n = esp_http_client_read(h, buf, content_len);
    if (n < 0) {
        n = 0;
    }
    buf[n] = '\0';                     /* 字符串化,方便后面查找/打印 */

    esp_http_client_close(h);
    esp_http_client_cleanup(h);
    return n;
}
