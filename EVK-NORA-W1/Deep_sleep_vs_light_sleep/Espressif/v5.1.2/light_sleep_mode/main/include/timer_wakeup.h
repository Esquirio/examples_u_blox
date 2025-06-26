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

#ifndef TIMER_WAKEUP

#define TIMER_WAKEUP
    
#include "esp_check.h"
#include "esp_sleep.h"
#include "esp_timer.h"

#define TIMER_WAKEUP_TIME_US    (2 * 1000 * 1000)

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t register_timer_wakeup(void);

#ifdef __cplusplus
}
#endif

#endif