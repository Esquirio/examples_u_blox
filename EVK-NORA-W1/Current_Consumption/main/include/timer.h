#ifndef __NORA_W1_TIMER_H__
#define __NORA_W1_TIMER_H__

#include "esp_timer.h"
#include "esp_sleep.h"
#include "esp_log.h"
#include "deep_sleep.h"
#include <unistd.h>

#define PERIODIC_TIMER       1*1000000UL //     1 second
#define ONE_SHOT_TIMER      75*1000000UL //    75 seconds

static const char* TAG_TIMER = "TIMER";

uint8_t selected_payload = 0;

extern void tcp_client_send(void);
extern void tcp_client_disconnect(void);
extern void config_power_save(bool light_sleep);

// Timer functions prototypes
static void periodic_timer_callback(void* arg);
static void oneshot_timer_callback(void* arg);

void timer_init(void);

// Variables
uint8_t timer_count = 0;
esp_timer_handle_t periodic_timer;
esp_timer_handle_t oneshot_timer;

#endif /* __NORA_W1_TIMER_H__ */