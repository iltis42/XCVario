set(COMPONENT_SRCS "src/coredump_to_server.cpp")
set(COMPONENT_ADD_INCLUDEDIRS "include")
set(COMPONENT_REQUIRES spi_flash app_update mbedtls esp_rom soc espcoredump)
register_component()
