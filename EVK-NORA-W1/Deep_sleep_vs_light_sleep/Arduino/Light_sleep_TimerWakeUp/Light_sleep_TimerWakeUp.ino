/*
Simple Light Sleep with Timer Wake Up
=====================================
NORA-W1 offers a light sleep mode for effective power
saving as power is an important factor for IoT
applications. In this mode CPUs, the digital 
peripherals, most of the RAM, and CPUs are 
clock-gated and their supply voltage is reduced. 
Upon exit from Light-sleep, the digital peripherals, 
RAM, and CPUs resume operation and their internal 
states are preserved.

This code displays the most basic light sleep with
a timer to wake it up.

This code is under Public Domain License.
*/

// #include <esp32/pm.h>
// #include <esp_pm.h>
#include "SimpleBLE.h"
extern "C" {
#include <esp_wifi.h>
#include <esp_bt_main.h>
#include <esp_sleep.h>
} //extern "C"

#define uS_TO_S_FACTOR 1000000ULL  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP  5           /* Time ESP32 will go to sleep (in seconds) */

/*
Method to print the reason by which ESP32
has been awaken from sleep
*/
void print_wakeup_reason(){
  esp_sleep_wakeup_cause_t wakeup_reason;

  wakeup_reason = esp_sleep_get_wakeup_cause();

  switch(wakeup_reason)
  {
    case ESP_SLEEP_WAKEUP_TIMER : Serial.println("Wakeup caused by timer"); break;
    default : Serial.printf("Wakeup was not caused by light sleep: %d\n",wakeup_reason); break;
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000); //Take some time to open up the Serial Monitor

  // Power down the following domains
  esp_sleep_pd_config(ESP_PD_DOMAIN_VDDSDIO,ESP_PD_OPTION_OFF);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_OFF);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_SLOW_MEM, ESP_PD_OPTION_OFF);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_FAST_MEM, ESP_PD_OPTION_OFF);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_CPU, ESP_PD_OPTION_OFF);
  // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC8M, ESP_PD_OPTION_OFF);
  // // esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH,ESP_PD_OPTION_OFF);//!< RTC IO, sensors and ULP co-processor
  // // esp_sleep_pd_config(ESP_PD_DOMAIN_XTAL,ESP_PD_OPTION_OFF);//!< XTAL oscillator

  // Before entering deep sleep or light sleep modes, applications must disable WiFi and BT using appropriate calls
  // the following functions:
  // https://docs.espressif.com/projects/esp-idf/en/v4.4/esp32/api-reference/system/sleep_modes.html#wifi-bt-and-sleep-modes
  esp_bluedroid_disable();
  esp_bt_controller_disable();
  esp_wifi_stop();

  esp_sleep_cpu_pd_low_init(true); // Recommended for esp32-s3 in light-sleep mode
}

void loop() {
  /*
  First we configure the wake up source
  We set our NORA-W1 to wake up every 5 seconds
  */
  esp_sleep_enable_timer_wakeup(TIME_TO_SLEEP * uS_TO_S_FACTOR);
  Serial.println("Setup NORA-W1 to sleep for every " + String(TIME_TO_SLEEP) +
  " Seconds");

  /*
  Go to light sleep-mode.
  */
  Serial.println("Going to light-sleep now");
  Serial.flush();

  delay(1000);
  // Enter in light-sleep mode
  esp_light_sleep_start();
}