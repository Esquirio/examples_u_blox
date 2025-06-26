#include "udp_client.h"

void connect_to_server(void) {
    struct sockaddr_in dest_addr;
    dest_addr.sin_addr.s_addr = inet_addr(host_ip);
    dest_addr.sin_family = AF_INET;
    dest_addr.sin_port = htons(PORT);

    udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
    if (udp_socket < 0) {
        ESP_LOGE(TAG_UDP, "Unable to create socket: errno %d", errno);
        vTaskDelete(NULL);
    } else {
        ESP_LOGI(TAG_UDP, "Socket created, connecting to %s:%d", host_ip, PORT);
    }

    if (connect(udp_socket, (struct sockaddr *)&dest_addr, sizeof(dest_addr)) < 0) {
        ESP_LOGE(TAG_UDP, "Socket unable to connect: errno %d", errno);
        close(udp_socket);
        vTaskDelete(NULL);
    } else {
        ESP_LOGI(TAG_UDP, "Successfully connected");
    }
}

void send_udp_message(const char *message) {
    if (udp_socket >= 0) {
        int err = send(udp_socket, message, strlen(message), 0);
        if (err < 0) {
            ESP_LOGE(TAG_UDP, "Error occurred during sending: errno %d", errno);
            vTaskDelete(NULL);
        } else {
            ESP_LOGI(TAG_UDP, "Message was sent successfully!");
        }
    } else {
        ESP_LOGE(TAG_UDP, "Socket not connected");
    }
}

void receive_udp_message(void *pvParameters) {
    char buffer[256];
    struct sockaddr_in source_addr;
    socklen_t socklen = sizeof(source_addr);

    while (true) {
        int len = recvfrom(udp_socket, buffer, sizeof(buffer) - 1, 0, (struct sockaddr *)&source_addr, &socklen);
        if (len < 0) {
            ESP_LOGE(TAG_UDP, "Receive failed");
            break;
        }
        buffer[len] = '\0';
        ESP_LOGI(TAG_UDP, "Received message from %s:%d - %s", inet_ntoa(source_addr.sin_addr), ntohs(source_addr.sin_port), buffer);
    }

    close(udp_socket);
    vTaskDelete(NULL);
}