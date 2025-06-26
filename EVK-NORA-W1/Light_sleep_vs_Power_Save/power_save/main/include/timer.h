#include "esp_timer.h"
#include "esp_sleep.h"
#include "esp_log.h"

#define TIMER_PERIOD       30000000

static const char* TAG_TIMER = "TIMER";

uint8_t selected_payload = 0;

extern void tcp_client_send(uint8_t selected_payload);

// Timer functions prototypes
static void periodic_timer_callback(void* arg);
void timer_init(void);