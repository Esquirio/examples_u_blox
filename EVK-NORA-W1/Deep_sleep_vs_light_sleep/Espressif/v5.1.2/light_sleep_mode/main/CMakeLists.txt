set(srcs "main.c"
         "light_sleep.c"
         "timer_wakeup.c")

set(TOUCH_ELEMENT_COMPATIBLE_TARGETS "esp32s2" "esp32s3")

idf_component_register(SRCS ${srcs}
                       INCLUDE_DIRS "include")
