#include "timer.h"

// Timer functions
void timer_init(void)
{
    /* Create timer:
     * 1. a periodic timer which will run every 30s, and print a message
     * 2. .
     */

    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback,
            /* name is optional, but may help identify the timer when debugging */
            .name = "Periodic TIMER"
    };

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    /* The timer has been created but is not running yet */

    /* Start the timer */
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, TIMER_PERIOD));
    ESP_LOGI(TAG_TIMER, "Started timers, time since boot: %lld us", esp_timer_get_time());

    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(TIMER_PERIOD));
}

static void periodic_timer_callback(void* arg)
{
    // Send the first message to establish the UDP connection
    const char *message = "Hello from NORA-W4";
    send_udp_message(message);
}