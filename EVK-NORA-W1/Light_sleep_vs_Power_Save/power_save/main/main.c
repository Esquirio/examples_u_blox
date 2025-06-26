/* 

*/

/*
   
*/
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_pm.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sys.h"

extern void wifi_init_sta(void);
extern void tcp_client_connect(void);
extern void tcp_client_receive(void *args);
extern void timer_init(void);

void app_main(void)
{
    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK( ret );

#if CONFIG_PM_ENABLE
    // Configure dynamic frequency scaling:
    // maximum and minimum frequencies are set in sdkconfig,
    // automatic light sleep is enabled if tickless idle support is enabled.
    esp_pm_config_t pm_config = {
            .max_freq_mhz = CONFIG_EVK_MAX_CPU_FREQ_MHZ,
            .min_freq_mhz = CONFIG_EVK_MIN_CPU_FREQ_MHZ,
#if CONFIG_FREERTOS_USE_TICKLESS_IDLE
            .light_sleep_enable = true
#endif
    };
    ESP_ERROR_CHECK( esp_pm_configure(&pm_config) );
#endif // CONFIG_PM_ENABLE

    wifi_init_sta();

    tcp_client_connect();

    timer_init();

    xTaskCreate(tcp_client_receive, "tcp_client_receive", 4096, NULL, 6, NULL);
}
