#include "timer.h"

// Timer functions
void timer_init(void)
{
    /* Create two timers:
     * 1. a periodic timer which will run every 1s, and send data to TCP Server
     * 2. a one-shot timer which will fire after 108s, and enter in deep-sleep mode.
     */

    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback,
            /* name is optional, but may help identify the timer when debugging */
            .name = "Periodic TIMER"
    };

    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    timer_count = 0;
    /* The timer has been created but is not running yet */

    const esp_timer_create_args_t oneshot_timer_args = {
            .callback = &oneshot_timer_callback,
            /* argument specified here will be passed to timer callback function */
            .arg = (void*) periodic_timer,
            .name = "one-shot"
    };

    ESP_ERROR_CHECK(esp_timer_create(&oneshot_timer_args, &oneshot_timer));

    /* Start the timers */
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, PERIODIC_TIMER));
    ESP_ERROR_CHECK(esp_timer_start_once(oneshot_timer, ONE_SHOT_TIMER));
    ESP_LOGI(TAG_TIMER, "Started timers, time since boot: %lld us", esp_timer_get_time());

    // ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(TIMER2_PERIOD));
}

static void periodic_timer_callback(void* arg)
{
    ESP_LOGI(TAG_TIMER, "Calling TCP sender");
    tcp_client_send();
}

static void oneshot_timer_callback(void* arg)
{
    int64_t time_since_boot = esp_timer_get_time();
    ESP_LOGI(TAG_TIMER, "One-shot timer called, time since boot: %lld us", time_since_boot);
    /* Clean up and finish the example */
    ESP_LOGI(TAG_TIMER, "Stopping and deleting timers");
    ESP_ERROR_CHECK(esp_timer_stop(periodic_timer));
    ESP_ERROR_CHECK(esp_timer_delete(periodic_timer));
    ESP_ERROR_CHECK(esp_timer_delete(oneshot_timer));
    
    usleep(0.3*1000000UL);

    ESP_LOGI(TAG_TIMER, "Disconnecting TCP Client");
    tcp_client_disconnect();

    ESP_LOGI(TAG_TIMER, "Entering in deep-sleep mode");
    // Add deep sleep-mode function here
    deep_sleep_task();
}
