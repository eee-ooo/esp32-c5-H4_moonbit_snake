# API 功能卡(每站一卡,随里程更新)

> 用法:复习时对着卡背函数 → 打开对应头文件看签名 → 心算调用链。
> 头文件主目录:`D:\eeeooo\esp32\v5.5.5\esp-idf\components\<组件名>\include\`

---

## 站 5+6:WiFi(扫描雷达 scan + 上车 join)

### A. 驱动层 — `components/esp_wifi/include/esp_wifi.h`

| 函数/宏 | 效果 | 一句话印象 |
|---|---|---|
| `WIFI_INIT_CONFIG_DEFAULT()` | 取一套驱动默认参数 | 所有初始化都是"默认宏打底" |
| `esp_wifi_init(&cfg)` | 加载 WiFi 驱动固件/开缓冲 | 开机只调用一次 |
| `esp_wifi_set_mode(WIFI_MODE_STA)` | 本机角色 = 客户端(STA) | STA = 手机/电脑,AP = 路由器 |
| `esp_wifi_start()` | 驱动开工(STA 就绪) | 之后 STA_START 事件才来 |
| `esp_wifi_scan_start(&cfg, true)` | 开始扫描;`true`=阻塞等完成 | 单命令=3~5 秒,期间命令行卡住 |
| `esp_wifi_scan_get_ap_num(&n)` | 数出身边几个 AP | 先数,再取 |
| `esp_wifi_scan_get_ap_records(&n, recs)` | 取记录数组(名字/RSSI/安全) | `wifi_ap_record_t` 数组 |
| `esp_wifi_set_config(WIFI_IF_STA, &cfg)` | 填"要连哪家"的 ssid/密码 | `wifi_config_t` 结构体 |
| `esp_wifi_connect()` | 发起连接请求 | 连接结果靠**事件**回传 |

### B. 网络接口层 — `components/esp_netif/include/esp_netif.h`

| 函数 | 效果 | 一句话印象 |
|---|---|---|
| `esp_netif_init()` | 网络协议栈接口层初始化 | 用 WiFi 前必调 |
| `esp_netif_create_default_wifi_sta()` | **创建 STA 网络接口实例(DHCP 客户端住所)** | 漏了它 = 连得上但永远拿不到 IP! |
| `IP_EVENT_STA_GOT_IP` | "拿到 IP"事件 | 联网成功的"毕业证书" |
| `ip_event_got_ip_t *` + `IP2STR/IPSTR` | 事件负载 + IP 转字符串 | 第 1 次看到"事件数据"怎么取 |

### C. 事件总线 — `components/esp_event/include/esp_event.h`

| 函数 | 效果 | 一句话印象 |
|---|---|---|
| `esp_event_loop_create_default()` | 建一个"广播站"(事件循环) | 事件制的地基 |
| `esp_event_handler_instance_register(基, 任意ID, 回调, ...)` | 注册监听器 | 写"回调函数"= 嵌入式新范式 |
| 事件: `WIFI_EVENT_STA_START/CONNECTED/DISCONNECTED` | 驱动状态变化的"广播" | DISCONNECTED→重连=救命模式 |

### D. 标准库(陪跑)

| 函数 | 效果 |
|---|---|
| `calloc(n, size)` / `free()` | 动态申请/释放记录数组 |
| `strncpy(dst, src, n)` | 安全拷贝(防溢出),填 SSID/密码 |

### 调用链(背出来)

```
join/scan → wifi_app_join/scan
  → init_once: netif_init → event_loop → 注册回调 → wifi_init → set_mode(STA) → start
  → set_config / scan_start
  → 系统后台: STA_START → connect → CONNECTED → GOT_IP(事件,自动进回调)
  ↓ 断线 DISCONNECTED → 回调里 esp_wifi_connect() = 自动重连
```

---

## 站 7:HTTP 取数(fetch / time)

### 组件 − `components/esp_http_client/include/esp_http_client.h`

| 函数 | 效果 | 一句话印象 |
|---|---|---|
| `esp_http_client_init(&cfg)` | 按配置打开一条 HTTP 会话 | 相当于"拿着地址去门口排队" |
| `esp_http_client_perform(h)` | 一步走完:握手+请求+收应答头 | 主力函数,一次搞定 |
| `esp_http_client_get_status_code(h)` | 服务器应答码(200=成功) | 先看成不成 |
| `esp_http_client_get_content_length(h)` | 应答体字节数 | 决定往缓冲区装多少 |
| `esp_http_client_read(h, buf, len)` | 真正掏出正文内容 | `len` 注意别超缓冲区 |
| `esp_http_client_close/cleanup` | 关闭会话/释放 | 有借有还 |

### 配置字段 `esp_http_client_config_t`
`.url`(要的地址)· `.method = HTTP_METHOD_GET`· `.timeout_ms = 8000`(超时)· `.max_redirection_count = 3`(允许跳转几次)
