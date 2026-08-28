idf_component_register(SRCS "cli.c"
                    INCLUDE_DIRS "."
                    REQUIRES console BSP nvs_flash esp_wifi esp_netif esp_event)
