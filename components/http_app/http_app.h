#ifndef HTTP_APP_H
#define HTTP_APP_H

#include <stddef.h>

/* 同步请求一个网页/接口,把响应体内容拷进缓冲区
 * 返回读取到的字节数;失败返回负数 */
int http_app_get(const char *url, char *buf, size_t buf_len);

#endif /* HTTP_APP_H */
