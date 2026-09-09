idf_component_register(
    SRCS "runtime.c"
         "framebuffer.c"
         "display.c"
         "util.c"
         "wamr.c"
         "strnlen.c"
         "gamecard.c"
         "control.c"
    INCLUDE_DIRS "."
    REQUIRES BSP wamr pthread
)

target_compile_options(${COMPONENT_LIB} PRIVATE
    -Wno-pointer-sign
    -Wno-error=format
    -Wno-error=return-type
    -Wno-error=unused-variable
    -Wno-error=type-limits
)
