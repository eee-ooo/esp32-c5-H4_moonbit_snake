# 易错点账本(pitfalls ledger)

> 规则:推图阶段翻车就记账(现象 / 根因 / 解法),不求甚解,先继续前进;
> 全部章节点完后,按本账本逐条巩固。
> 状态:✅ 已修复已验证 / ⚠️ 概念坑(要理解) / 📌 待深挖

## 已归档(2026-08-26~27,第一课前后)

| # | 现象 | 根因 | 解法 | 状态 |
|---|---|---|---|---|
| 1 | 编译报 `EXTRA_COMPONENT_DIRS doesn't exist` | 复制工程带了一条指向不存在目录的声明(实际目录名是 Middlewares,且此声明本来就多余) | 删除该行(IDF v5.5 对不存在的目录直接 FATAL) | ✅ |
| 2 | 跳转/智能提示全乱,指到别的工程 | settings.json 的 clangd `compile-commands-dir` 是复制残留的绝对路径 | 改成 `${workspaceFolder}/build` | ✅ |
| 3 | 明明改了代码,重新编译却看不到变化 | 复制来的 build/ 里 CMakeCache 记录着**源工程路径**(CMAKE_HOME_DIRECTORY) | 拷贝工程后**删 build/** 重新构建 | ✅ |
| 4 | 文件在别人(或 Linux)平台上"消失" | BSP 的 CMakeLists 文件名大小写错误:`CMakelists.txt` | 一切 esp-idf 文件:**CMakeLists.txt**(Lists 的 L 大写) | ✅ |
| 5 | 我改的磁盘内容被"打回原形" | IDE 里旧路径标签(改名后的文件)缓存被 Ctrl+S 写回 | 改名/删除后,**关旧标签或 Revert File**,再操作 | ✅ |
| 6 | 开机即崩溃:`abort(){mspi_init, cpu_start.c:616}` | 拷贝来的 sdkconfig 开着 `CONFIG_SPIRAM=y`(板子无 PSRAM),且未勾 IGNORE_NOTFOUND → 启动初始化 PSRAM 失败即 abort | menuconfig 关闭 SPIRAM;**教训:换板子必查 sdkconfig**(它不进 git,换环境易复发) | ✅ |
| 7 | 调试器监视 `test` 报 `<var-create: unable to create variable object>` | 变量在源码里,但 ELF 里没有——**改了没编译/没烧** | 改代码 → Build → Flash → 再调试 | ✅ |
| 8 | 万用表读 0.4V/0.1V "假信号" | 引脚未配置(悬空)时读到的漂浮电压,不是逻辑值 | 先看 Monitor 日志确认程序在跑,再谈电压 | ✅ |
| 9 | `bitq`/`dtmcs`/`polling failed` 刷屏,IDE 设置卡死 | 残留 `openocd.exe` 僵尸进程独占 USB-JTAG 口,新连接掐断通信 | **先杀进程(`taskkill /F /IM openocd.exe`),再拔插 USB 线**;调试器一次只开一个 | ✅ |
| 10 | `Failed to resolve component 'esp_console'` | 组件名(`console`)≠ 头文件前缀(`esp_console`)≠API 前缀(`esp_console_*`);REQUIRES 只认组件名 | `REQUIRES console`(调试:看 `components/console/` 目录名) | ✅ |
| 11 | `scan` 报 `0xffffffff (ESP_FAIL)`:`STA is connecting, scan are not allowed!` | 事件回调在 `STA_START` 里**自动** connect,抢在扫描前把驱动带进"连接中"状态 | 回调要"意图门控":定义 `s_join_requested` 标志,只有 join 下令过才允许自动重连;START 事件只登记不动作 | ✅ |
| 12 | 能连上 AP(`connected to AP`)但**永远无 GOT_IP**(等 40s、换频段都一样) | 漏了 `esp_netif_create_default_wifi_sta()`:没有 STA netif 实例 = 没有 DHCP 客户端 = 没人领 IP。**分层诊断法**:scan 正常+connected 正常 = 射频层无恙 → 问题必然在 IP 层 | WiFi 四件套必须齐:`esp_netif_init() → esp_event_loop_create_default() → esp_netif_create_default_wifi_sta() → esp_wifi_init()` | ✅ |

## 概念坑(推图时留意)

| # | 坑 | 一句话 |
|---|---|---|
| C1 | `pdMS_TO_TICKS(333)` ≠ 333ms | tick 量化:333ms → 33 tick → **330ms** |
| C2 | "任务=时间片轮询"的误区 | FreeRTOS 主力是**阻塞唤醒 + 优先级抢占**;时间片只是同优先级兜底 |
| C3 | 优先级饿死(Starvation) | 高优先级任务不睡眠 ⇒ 低优先级永远没 CPU(实验待做) |
| C4 | `1 << n` 与 `1ULL << n` | 位掩码用 64 位无符号;`1<<40` 在 32 位上是未定义行为 |
| C5 | 日志级别是编译期闸门 | menuconfig 的 LOG level 决定 D/V 级日志是否被切掉 |
| C6 | 组件名=目录名,大小写敏感 | `components/BSP` → `REQUIRES BSP` |
| C7 | REPL 也是一个任务 | `esp_console_start_repl()` 内部自建任务,不停机 |
| C8 | 组件名不能与系统组件撞名 | 自定义 CLI 组件不能叫 `console`(系统已有),这里用 `cli` |
| C10 | USB-SJ 虚拟串口的 DTR/RTS = 隐形"复位+下载模式"开关 | 普通串口助手打开/断开时会乱拉 DTR/RTS,把芯片按进 ROM 下载模式 → 程序没跑 → 引脚浮空;微软 Serial Monitor 不碰它们 → 正常。解法:助手配置里**取消勾选 DTR/RTS**;浮空时拔插 Type-C 或重连监视器即"复活" |
| C9 | 控制台"主/副"分家:输入只走主,UART 未接线则 REPL 是聋子 | MZ56:主=UART0(GPIO11/12 排针孔,裸板没接),副=USB-SJ(COM6 只镜像输出);日志看得见、命令打不进 → 主控制台必须切成 USB-SJ(menuconfig:Console → USB Serial/JTAG),一切换,`esp_console_new_repl_usb_serial_jtag` API 也被开关放行(该 API 有 `#if CONFIG_ESP_CONSOLE_USB_SERIAL_JTAG` 守卫) |

## 待补(推图完成后)

- [ ] 饿死任务实验(优先级 7 + 空转 vs vTaskDelay)
- [ ] 每个章节的"翻车记录"(推到哪记到哪)
