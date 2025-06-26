/*
 * Copyright 2023 u-blox
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *  http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "light_sleep.h"

static const char* TAG_LIGHT = "Light Sleep";

void light_sleep_task(void *args)
{
    while (true) {
        ESP_LOGI(TAG_LIGHT, "Entering light-sleep mode");

        // Necessary for NORA-W1 (esp32-s3) to get the right current
        // consumption during the light-sleep mode.
        ESP_ERROR_CHECK(esp_sleep_cpu_pd_low_init());

        /* Enter sleep mode */
        esp_light_sleep_start();
    }
    vTaskDelete(NULL);
}