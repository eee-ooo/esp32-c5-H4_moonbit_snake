idf_component_register(SRCS "cli.c"
                    INCLUDE_DIRS "."
                    REQUIRES console BSP nvs_flash wifi_app http_app)
