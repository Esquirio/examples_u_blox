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

    wifi_init_sta();

    tcp_client_connect();

    timer_init();
}
