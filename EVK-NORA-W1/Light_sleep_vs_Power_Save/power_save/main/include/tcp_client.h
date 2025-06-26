#include "sdkconfig.h"
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <errno.h>
#include <netdb.h>            // struct addrinfo
#include <arpa/inet.h>
#include "esp_netif.h"
#include "esp_log.h"
#if defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
#include "addr_from_stdin.h"
#endif

#if defined(CONFIG_EXAMPLE_IPV4)
#define HOST_IP_ADDR CONFIG_EXAMPLE_IPV4_ADDR
#elif defined(CONFIG_EXAMPLE_SOCKET_IP_INPUT_STDIN)
#define HOST_IP_ADDR ""
#endif

#define PORT CONFIG_EXAMPLE_PORT

static const char* TAG_TCP = "TCP Client";

char rx_buffer[4000];
char host_ip[] = HOST_IP_ADDR;
int addr_family = 0;
int ip_protocol = 0;
int sock;
int err;

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

void tcp_client_connect(void);
void tcp_client_send(uint8_t selected_payload);
void tcp_client_disconnect(void);
void tcp_client_receive(void *args);
