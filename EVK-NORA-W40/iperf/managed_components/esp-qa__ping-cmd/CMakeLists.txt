set(srcs  "src/ping_cmd.c")
set(priv_requires console esp_netif lwip)

idf_component_register(SRCS "${srcs}"
                    INCLUDE_DIRS "include"
                    PRIV_REQUIRES ${priv_requires})

if(CMAKE_CXX_COMPILER_VERSION VERSION_GREATER 11.0)
    target_compile_options(${COMPONENT_LIB} PRIVATE "-Wno-format")
endif()
