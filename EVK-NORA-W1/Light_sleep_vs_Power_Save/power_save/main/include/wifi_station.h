#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_log.h"
#include "esp_event.h"
#include "esp_pm.h"
#include "nvs_flash.h"

/*set the ssid and password via "idf.py menuconfig"*/
#define WIFI_SSID           CONFIG_EVK_WIFI_SSID
#define WIFI_PASSWORD       CONFIG_EVK_WIFI_PASSWORD
#define WIFI_MAXIMUM_RETRY  CONFIG_EVK_MAXIMUM_RETRY

#define WIFI_LISTEN_INTERVAL CONFIG_EVK_WIFI_LISTEN_INTERVAL
#define WIFI_BEACON_TIMEOUT  CONFIG_EVK_WIFI_BEACON_TIMEOUT

#if CONFIG_EVK_WIFI_AUTH_OPEN
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_EVK_WIFI_AUTH_WEP
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_EVK_WIFI_AUTH_WPA_PSK
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_EVK_WIFI_AUTH_WPA2_PSK
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_EVK_WIFI_AUTH_WPA_WPA2_PSK
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_EVK_WIFI_AUTH_WPA3_PSK
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_EVK_WIFI_AUTH_WPA2_WPA3_PSK
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_EVK_WIFI_AUTH_WAPI_PSK
#define WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

#if CONFIG_EVK_POWER_SAVE_MIN_MODEM
#define PS_MODE WIFI_PS_MIN_MODEM
#elif CONFIG_EVK_POWER_SAVE_MAX_MODEM
#define PS_MODE WIFI_PS_MAX_MODEM
#elif CONFIG_EVK_POWER_SAVE_NONE
#define PS_MODE WIFI_PS_NONE
#else
#define PS_MODE WIFI_PS_NONE
#endif /*CONFIG_POWER_SAVE_MODEM*/

static const char *TAG_WIFI_STA = "wifi_sta_ps";

/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1

static int s_retry_num = 0;

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);

void wifi_init_sta(void);