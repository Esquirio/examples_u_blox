if(${IDF_TARGET} STREQUAL "linux")
    set(requires esp_stubs protocol_examples_common nvs_flash)
endif()

set(srcs "nora_w1.c"
         "tcp_client_v4.c")

idf_component_register(SRCS ${srcs}
                        INCLUDE_DIRS "."
                        REQUIRES ${requires})
