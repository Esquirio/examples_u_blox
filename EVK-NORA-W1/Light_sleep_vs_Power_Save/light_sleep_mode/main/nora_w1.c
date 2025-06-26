/* 
  .
*/
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <netdb.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "esp_timer.h"
#include "esp_sleep.h"
#include "sdkconfig.h"

#include "lwip/err.h"
#include "lwip/sys.h"

// #include "protocol_examples_common.h"

#define ESP_WIFI_SSID             CONFIG_ESP_WIFI_SSID
#define ESP_WIFI_PASS             CONFIG_ESP_WIFI_PASSWORD
#define ESP_MAXIMUM_RETRY         CONFIG_ESP_MAXIMUM_RETRY
#define ESP_STATIC_IP_ADDR        CONFIG_ESP_STATIC_IP_ADDR
#define ESP_STATIC_NETMASK_ADDR   CONFIG_ESP_STATIC_NETMASK_ADDR
#define ESP_STATIC_GW_ADDR        CONFIG_ESP_STATIC_GW_ADDR

#if CONFIG_ESP_WIFI_AUTH_OPEN
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_OPEN
#elif CONFIG_ESP_WIFI_AUTH_WEP
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WEP
#elif CONFIG_ESP_WIFI_AUTH_WPA_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA_WPA2_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA_WPA2_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WPA2_WPA3_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WPA2_WPA3_PSK
#elif CONFIG_ESP_WIFI_AUTH_WAPI_PSK
#define ESP_WIFI_SCAN_AUTH_MODE_THRESHOLD WIFI_AUTH_WAPI_PSK
#endif

#ifdef CONFIG_ESP_STATIC_DNS_AUTO
#define ESP_MAIN_DNS_SERVER       ESP_STATIC_GW_ADDR
#define ESP_BACKUP_DNS_SERVER     "0.0.0.0"
#else
#define ESP_MAIN_DNS_SERVER       CONFIG_ESP_STATIC_DNS_SERVER_MAIN
#define ESP_BACKUP_DNS_SERVER     CONFIG_ESP_STATIC_DNS_SERVER_BACKUP
#endif

#ifdef CONFIG_ESP_STATIC_DNS_RESOLVE_TEST
#define ESP_RESOLVE_DOMAIN        CONFIG_ESP_STATIC_RESOLVE_DOMAIN
#endif


/* FreeRTOS event group to signal when we are connected*/
static EventGroupHandle_t s_wifi_event_group;

/* The event group allows multiple bits for each event, but we only care about two events:
 * - we are connected to the AP with an IP
 * - we failed to connect after the maximum amount of retries */
#define WIFI_CONNECTED_BIT BIT0
#define WIFI_FAIL_BIT      BIT1
#define TIMER_PERIOD       25000000

static const char *TAG = "NORA-W1";

static int s_retry_num = 0;

uint8_t selected_payload = 1;

// Wi-Fi functions prototypes
void wifi_init_sta(void);
static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data);
static esp_err_t esp_set_dns_server(esp_netif_t *netif,
                          uint32_t addr, esp_netif_dns_type_t type);
static void esp_set_static_ip(esp_netif_t *netif);

static void light_sleep_task(void *args);

// Timer functions prototypes
static void periodic_timer_callback(void* arg);
void timers_init(void);

extern void tcp_client_send(uint8_t select_payload);

void app_main(void)
{
    //Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
      ESP_ERROR_CHECK(nvs_flash_erase());
      ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // ESP_ERROR_CHECK(example_connect());
    wifi_init_sta();

    timers_init();

    usleep(5 * 1000 * 1000);

    xTaskCreate(light_sleep_task, "light_sleep_task", 4096, NULL, 6, NULL);
}

static void light_sleep_task(void *args)
{
    while (true) {
        printf("Entering light sleep\n");
        /* Get timestamp before entering sleep */
        int64_t t_before_us = esp_timer_get_time();

        // Necessary for NORA-W1 (esp32-s3) to get the right current
        // consumption during the light-sleep mode.
        ESP_ERROR_CHECK(esp_sleep_cpu_pd_low_init());

        /* Enter sleep mode */
        esp_light_sleep_start();

        /* Get timestamp after waking up from sleep */
        int64_t t_after_us = esp_timer_get_time();

        /* Determine wake up reason */
        const char* wakeup_reason;
        switch (esp_sleep_get_wakeup_cause()) {
            case ESP_SLEEP_WAKEUP_TIMER:
                usleep(10 * 1000 * 1000);
                tcp_client_send(selected_payload);
                wakeup_reason = "Timer";
                usleep(10 * 1000 * 1000);
                // vTaskDelay(10);
                break;
            default:
                wakeup_reason = "other";
                break;
        }
        printf("Returned from light sleep, reason: %s, t=%lld ms, slept for %lld ms\n",
                wakeup_reason, t_after_us / 1000, (t_after_us - t_before_us) / 1000);
    }
    vTaskDelete(NULL);
}

// Wi-Fi functions
static esp_err_t esp_set_dns_server(esp_netif_t *netif, uint32_t addr, esp_netif_dns_type_t type)
{
    if (addr && (addr != IPADDR_NONE)) {
        esp_netif_dns_info_t dns;
        dns.ip.u_addr.ip4.addr = addr;
        dns.ip.type = IPADDR_TYPE_V4;
        ESP_ERROR_CHECK(esp_netif_set_dns_info(netif, type, &dns));
    }
    return ESP_OK;
}

static void esp_set_static_ip(esp_netif_t *netif)
{
    if (esp_netif_dhcpc_stop(netif) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to stop dhcp client");
        return;
    }
    esp_netif_ip_info_t ip;
    memset(&ip, 0 , sizeof(esp_netif_ip_info_t));
    ip.ip.addr = ipaddr_addr(ESP_STATIC_IP_ADDR);
    ip.netmask.addr = ipaddr_addr(ESP_STATIC_NETMASK_ADDR);
    ip.gw.addr = ipaddr_addr(ESP_STATIC_GW_ADDR);
    if (esp_netif_set_ip_info(netif, &ip) != ESP_OK) {
        ESP_LOGE(TAG, "Failed to set ip info");
        return;
    }
    ESP_LOGD(TAG, "Success to set static ip: %s, netmask: %s, gw: %s", ESP_STATIC_IP_ADDR, ESP_STATIC_NETMASK_ADDR, ESP_STATIC_GW_ADDR);
    ESP_ERROR_CHECK(esp_set_dns_server(netif, ipaddr_addr(ESP_MAIN_DNS_SERVER), ESP_NETIF_DNS_MAIN));
    ESP_ERROR_CHECK(esp_set_dns_server(netif, ipaddr_addr(ESP_BACKUP_DNS_SERVER), ESP_NETIF_DNS_BACKUP));
}

void wifi_init_sta(void)
{
    s_wifi_event_group = xEventGroupCreate();

    ESP_ERROR_CHECK(esp_netif_init());

    ESP_ERROR_CHECK(esp_event_loop_create_default());

    esp_netif_t *sta_netif = esp_netif_create_default_wifi_sta();
    assert(sta_netif);

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        sta_netif,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        sta_netif,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = {
        .sta = {
            .ssid = ESP_WIFI_SSID,
            .password = ESP_WIFI_PASS,
            /* Setting a password implies station will connect to all security modes including WEP/WPA.
             * However these modes are deprecated and not advisable to be used. Incase your Access point
             * doesn't support WPA2, these mode can be enabled by commenting below line */
	     .threshold.authmode = WIFI_AUTH_WPA2_PSK,
        },
    };
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA) );
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config) );
    ESP_ERROR_CHECK(esp_wifi_start() );

    ESP_LOGI(TAG, "wifi_init_sta finished.");

    /* Waiting until either the connection is established (WIFI_CONNECTED_BIT) or connection failed for the maximum
     * number of re-tries (WIFI_FAIL_BIT). The bits are set by event_handler() (see above) */
    EventBits_t bits = xEventGroupWaitBits(s_wifi_event_group,
            WIFI_CONNECTED_BIT | WIFI_FAIL_BIT,
            pdFALSE,
            pdFALSE,
            portMAX_DELAY);

    /* xEventGroupWaitBits() returns the bits before the call returned, hence we can test which event actually
     * happened. */
    if (bits & WIFI_CONNECTED_BIT) {
        ESP_LOGI(TAG, "connected to ap SSID:%s password:%s",
                 ESP_WIFI_SSID, ESP_WIFI_PASS);
    } else if (bits & WIFI_FAIL_BIT) {
        ESP_LOGI(TAG, "Failed to connect to SSID:%s, password:%s",
                 ESP_WIFI_SSID, ESP_WIFI_PASS);
    } else {
        ESP_LOGE(TAG, "UNEXPECTED EVENT");
    }
#ifdef CONFIG_ESP_STATIC_DNS_RESOLVE_TEST
    struct addrinfo *address_info;
    struct addrinfo hints;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;

    int res = getaddrinfo(ESP_RESOLVE_DOMAIN, NULL, &hints, &address_info);
    if (res != 0 || address_info == NULL) {
        ESP_LOGE(TAG, "couldn't get hostname for :%s: "
                      "getaddrinfo() returns %d, addrinfo=%p", ESP_RESOLVE_DOMAIN, res, address_info);
    } else {
        if (address_info->ai_family == AF_INET) {
            struct sockaddr_in *p = (struct sockaddr_in *)address_info->ai_addr;
            ESP_LOGI(TAG, "Resolved IPv4 address: %s", ipaddr_ntoa((const ip_addr_t*)&p->sin_addr.s_addr));
        }
#if CONFIG_LWIP_IPV6
        else if (address_info->ai_family == AF_INET6) {
            struct sockaddr_in6 *p = (struct sockaddr_in6 *)address_info->ai_addr;
            ESP_LOGI(TAG, "Resolved IPv6 address: %s", ip6addr_ntoa((const ip6_addr_t*)&p->sin6_addr));
        }
#endif
    }
#endif
    /* The event will not be processed after unregister */
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(IP_EVENT, IP_EVENT_STA_GOT_IP, instance_got_ip));
    ESP_ERROR_CHECK(esp_event_handler_instance_unregister(WIFI_EVENT, ESP_EVENT_ANY_ID, instance_any_id));
    vEventGroupDelete(s_wifi_event_group);
}

static void event_handler(void* arg, esp_event_base_t event_base,
                                int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED) {
        esp_set_static_ip(arg);
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        if (s_retry_num < ESP_MAXIMUM_RETRY) {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "retry to connect to the AP");
        } else {
            xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        }
        ESP_LOGI(TAG,"connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        ESP_LOGI(TAG, "static ip:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xEventGroupSetBits(s_wifi_event_group, WIFI_CONNECTED_BIT);
    }
}

// Timer functions
void timers_init(void)
{
    /* Create timer:
     * 1. a periodic timer which will run every 30s, and print a message
     * 2. .
     */

    const esp_timer_create_args_t periodic_timer_args = {
            .callback = &periodic_timer_callback,
            /* name is optional, but may help identify the timer when debugging */
            .name = "Periodic TIMER1"
    };

    esp_timer_handle_t periodic_timer;
    ESP_ERROR_CHECK(esp_timer_create(&periodic_timer_args, &periodic_timer));
    /* The timer has been created but is not running yet */

    /* Start the timer */
    ESP_ERROR_CHECK(esp_timer_start_periodic(periodic_timer, TIMER_PERIOD));
    ESP_LOGI(TAG, "Started timers, time since boot: %lld us", esp_timer_get_time());

    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(TIMER_PERIOD));
}

static void periodic_timer_callback(void* arg)
{
    if(selected_payload < 10)
        selected_payload++;
    else
        selected_payload = 1;
}