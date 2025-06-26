/*
    
 */
#include "tcp_client.h"

void tcp_client_connect(void)
{
    while (1) 
    {
#if defined(CONFIG_EXAMPLE_IPV4)
        struct sockaddr_in dest_addr;
        inet_pton(AF_INET, host_ip, &dest_addr.sin_addr);
        dest_addr.sin_family = AF_INET;
        dest_addr.sin_port = htons(PORT);
        addr_family = AF_INET;
        ip_protocol = IPPROTO_IP;
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
        struct sockaddr_storage dest_addr = { 0 };
        ESP_ERROR_CHECK(get_addr_from_stdin(PORT, SOCK_STREAM, &ip_protocol, &addr_family, &dest_addr));
#endif

        sock =  socket(addr_family, SOCK_STREAM, ip_protocol);
        if (sock < 0) {
            ESP_LOGE(TAG_TCP, "Unable to create socket: errno %d", errno);
        } else {
            ESP_LOGI(TAG_TCP, "Socket created, connecting to %s:%d", host_ip, PORT);
        }

        int err = connect(sock, (struct sockaddr *)&dest_addr, sizeof(dest_addr));
        if (err != 0) {
            ESP_LOGE(TAG_TCP, "Socket unable to connect: errno %d", errno);
        } else {
            ESP_LOGI(TAG_TCP, "Successfully connected");
            break;
        }
    }
}

void tcp_client_send(void)
{
    while (1) 
    {
        err = send(sock, payload, strlen(payload), 0);
        
        if (err < 0) {
            ESP_LOGE(TAG_TCP, "Error occurred during sending: errno %d", errno);
            tcp_client_disconnect();
            tcp_client_connect();
        } else {
            ESP_LOGI(TAG_TCP, "Payload was sent successfully!");
            break;
        }
    }
}

void tcp_client_disconnect(void)
{
    if (sock != -1) {
        ESP_LOGE(TAG_TCP, "Shutting down socket ...");
        shutdown(sock, 0);
        close(sock);
    }
}

void tcp_client_receive(void *args)
{
    while (true) {
        int len = recv(sock, rx_buffer, sizeof(rx_buffer) - 1, 0);
        // Error occurred during receiving
        if (len < 0) {
            ESP_LOGE(TAG_TCP, "recv failed: errno %d", errno);
            break;
        }
        // Data received
        else {
            rx_buffer[len] = 0; // Null-terminate whatever we received and treat like a string
            ESP_LOGI(TAG_TCP, "Received %d bytes from %s:", len, host_ip);
            ESP_LOGI(TAG_TCP, "%s", rx_buffer);
        }
    }
    vTaskDelete(NULL);
    tcp_client_disconnect();
}
