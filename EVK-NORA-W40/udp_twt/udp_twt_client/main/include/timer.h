#include "esp_timer.h"
#include "esp_sleep.h"
#include "esp_log.h"

#define TIMER_PERIOD       120*1000*1000 // 2 minutes

static const char* TAG_TIMER = "TIMER";

uint8_t selected_payload = 0;

void send_udp_message(const char *message);

// Timer functions prototypes
static void periodic_timer_callback(void* arg);
void timer_init(void);