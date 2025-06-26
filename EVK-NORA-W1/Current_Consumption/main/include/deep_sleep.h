#ifndef __NORA_W1_DEEP_H__
#define __NORA_W1_DEEP_H__

#include <stdio.h>
#include <time.h>
#include <sys/time.h>
#include "sdkconfig.h"
#include "soc/soc_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "driver/rtc_io.h"
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_wifi.h"

void deep_sleep_task(void);

#endif /* __NORA_W1_DEEP_H__ */