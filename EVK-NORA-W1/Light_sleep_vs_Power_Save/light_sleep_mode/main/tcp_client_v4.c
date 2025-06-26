/*
 * SPDX-FileCopyrightText: 2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */
#include "sdkconfig.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>            // struct addrinfo
#include <arpa/inet.h>
#include "esp_netif.h"
#include "esp_log.h"
#if defined(CONFIG_ESP_SOCKET_IP_INPUT_STDIN)
#include "addr_from_stdin.h"
#endif

#if defined(CONFIG_ESP_IPV4)
#define HOST_IP_ADDR CONFIG_ESP_IPV4_ADDR
#elif defined(CONFIG_ESP_SOCKET_IP_INPUT_STDIN)
#define HOST_IP_ADDR ""
#endif

#define PORT CONFIG_ESP_PORT
static const char* TAG_TCP = "TCP Client";

static const char *payload_300B = "****** 300 Bytes message from the device to TCP sever - u-blox! ***** u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox";

static const char *payload_2kB = "****** 2 KBytes message from the device to TCP sever - u-blox! ***** u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox u-blox\
 u-blox u-blox";

void tcp_client_send(uint8_t select_payload)
{
    char host_ip[] = HOST_IP_ADDR;
    int addr_family = 0;
    int ip_protocol = 0;

    // while (1) 
    {
#if defined(CONFIG_ESP_IPV4)
        struct sockaddr_in dest_addr;
        inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
#elif defined(CONFIG_ESP_SOCKET_IP_INPUT_STDIN)
        struct sockaddr_storage dest_addr = { 0 };
        ESP_ERROR_CHECK(get_addr_from_stdin(PORT, SOCK_STREAM, &ip_protocol, &addr_family, &dest_addr));
#endif

        int sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG_TCP, "Unable to create socket: errno %d", errno);
            // break;
        }
        ESP_LOGI(TAG_TCP, "Socket created, connecting to %s:%d", host_ip, PORT);

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG_TCP, "Socket unable to connect: errno %d", errno);
            // break;
        }
        ESP_LOGI(TAG_TCP, "Successfully connected");
        if(select_payload < 10)
            err = send(sock, payload_300B, strlen(payload_300B), 0);
        else
            err = send(sock, payload_2kB, strlen(payload_2kB), 0);
        
        if (err < 0) {
            ESP_LOGE(TAG_TCP, "Error occurred during sending: errno %d", errno);
            // break;
        }

        if (sock != -1) {
            ESP_LOGE(TAG_TCP, "Closing the TCP socket ...");
            shutdown(sock, 0);
            close(sock);
        }
    }
}
