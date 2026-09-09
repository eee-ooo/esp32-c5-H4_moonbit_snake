# # BSP 组件:内部包含 LED 模块
# # 教程风格:先声明变量,再由 idf_component_register() 完成登记
# set(src_dirs LED)
# set(include_dirs LED)
# # 教程写的是 REQUIRES driver:v5.5.5 中仍可用但已弃用,推荐拆分后的 esp_driver_gpio
# set(requires esp_driver_gpio)

# idf_component_register(SRC_DIRS ${src_dirs} INCLUDE_DIRS ${include_dirs} REQUIRES ${requires})

# # 编译选项:-O3 最高优化;-ffast-math 放宽浮点严格性换速度;后两项关闭 printf 格式串警告
# component_compile_options(-ffast-math -O3 -Wno-error=format -Wno-format)
idf_component_register(
    SRCS "LED/led.c"           # 明确写出源文件（如果有多个文件，就继续往后加）
    INCLUDE_DIRS "LED"          # 明确写出包含目录
    REQUIRES esp_driver_gpio
)