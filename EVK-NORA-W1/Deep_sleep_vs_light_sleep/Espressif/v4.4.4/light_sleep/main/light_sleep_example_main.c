/* Light sleep example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <sys/time.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "esp_wifi.h"

void app_main(void)
{
    while (true) {
        /* Wake up in 2 seconds, or when button is pressed */
        esp_sleep_enable_timer_wakeup(5000000);

        printf("Entering light sleep\n");
        /* To make sure the complete line is printed before entering sleep mode,
         * need to wait until UART TX FIFO is empty:
         */
        uart_wait_tx_idle_polling(CONFIG_ESP_CONSOLE_UART_NUM);

        // Before entering deep sleep or light sleep modes, applications must disable WiFi and BT using appropriate calls
        // the following functions:
        // https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32/api-reference/system/sleep_modes.html#wifi-bt-and-sleep-modes
        // esp_bluedroid_disable();       // Not necessary - CONFIG_BT_ENABLED is not set
        // esp_bt_controller_disable();   // Not necessary - CONFIG_BT_ENABLED is not set
        esp_wifi_stop();

        esp_sleep_cpu_pd_low_init(true); // Recommended for esp32-s3 in light-sleep mode

        /* Get timestamp before entering sleep */
        int64_t t_before_us = esp_timer_get_time();

        /* Enter sleep mode */
        esp_light_sleep_start();
        /* Execution continues here after wakeup */

        /* Get timestamp after waking up from sleep */
        int64_t t_after_us = esp_timer_get_time();

        /* Determine wake up reason */
        const char* wakeup_reason;
        switch (esp_sleep_get_wakeup_cause()) {
            case ESP_SLEEP_WAKEUP_TIMER:
                wakeup_reason = "timer";
                break;
            default:
                wakeup_reason = "other";
                break;
        }

        printf("Returned from light sleep, reason: %s, t=%lld ms, slept for %lld ms\n",
                wakeup_reason, t_after_us / 1000, (t_after_us - t_before_us) / 1000);
    }

}
