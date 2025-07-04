| Supported Targets | ESP32 | ESP32-C2 | ESP32-C3 | ESP32-C6 | ESP32-S2 | ESP32-S3 |
| ----------------- | ----- | -------- | -------- | -------- | -------- | -------- |

# Deep Sleep Example

(See the README.md file in the upper level 'examples' directory for more information about examples.)

The [deep sleep mode](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/sleep_modes.html#sleep-modes) is a power saving mode that causes the CPU, majority of RAM, and digital peripherals that are clocked from APB_CLK to be powered off. Deep sleep mode can be exited using one of multiple [wake up sources](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/sleep_modes.html#wakeup-sources). This example demonstrates how to use the [`esp_sleep.h`](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/sleep_modes.html#api-reference) API to enter deep sleep mode, then wake up form different sources.

The following wake up sources are demonstrated in this example (refer to the [Wakeup Sources documentation](https://docs.espressif.com/projects/esp-idf/en/latest/api-reference/system/sleep_modes.html#wakeup-sources) for more details regarding wake up sources):

- **Timer:** An RTC timer that can be programmed to trigger a wake up after a preset time. This example will trigger a wake up every 20 seconds.

Note: Some wake up sources can be disabled via configuration (see section on [project configuration](#Configure-the-project))

Warning: On ESP32, touch wake up source cannot be used together with EXT0 or ULP wake up source. If they co-exist, IDF will give a runtime error and the program will crash. By default in this example, touch wake up is enabled, and the other two are disabled. You can switch to enable the other wake up sources via menuconfig.

In this example, the `CONFIG_BOOTLOADER_SKIP_VALIDATE_IN_DEEP_SLEEP` Kconfig option is used, which allows you to reduce the boot time of the bootloader during waking up from deep sleep. The bootloader stores in rtc memory the address of a running partition and uses it when it wakes up. This example allows you to skip all image checks and speed up the boot.

## How to use example

### Hardware Required

This example should be able to run on any commonly available ESP32 series development board without any extra hardware if only **Timer** and **ULP** wake up sources are used. However, the following extra connections will be required for the remaining wake up sources.

### Configure the project

```
idf.py menuconfig
```

* **Touch wake up** can be enabled/disabled via `Example configuration > Enable touch wake up`
* **EXT0 wake up** can be enabled/disabled via `Example configuration > Enable wakeup from GPIO (ext0)`
* **EXT1 wake up** can be enabled/disabled via `Example configuration > Enable wakeup from GPIO (ext1)`
* **GPIO wake up** can be enabled/disabled via `Example configuration > Enable wakeup from GPIO`
  Trigger pin can be chosen via `Example configuration > GPIO wakeup configuration > Enable wakeup from GPIO`
  Trigger level can be selected via `Example configuration > GPIO wakeup configuration > Enable GPIO high-level wakeup`
* **ULP wake up** can be enabled/disabled via `Example configuration > Enable temperature monitoring by ULP`

Wake up sources that are unused or unconnected should be disabled in configuration to prevent inadvertent triggering of wake up as a result of floating pins.

### Build and Flash

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

On initial startup, this example will detect that this is the first boot and output the following log:

```
...
I (304) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
Not a deep sleep reset
Enabling timer wakeup, 20s
Enabling EXT1 wakeup on pins GPIO2, GPIO4
Touch pad #8 average: 2148, wakeup threshold set to 2048.
Touch pad #9 average: 2148, wakeup threshold set to 2048.
Enabling touch pad wakeup
Enabling ULP wakeup
Entering deep sleep
```

The ESP32 will then enter deep sleep. When a wake up occurs, the ESP32 must undergo the entire boot process again. However the example will detect that this boot is due to a wake up and indicate the wake up source in the output log such as the following:

```
...
I (304) cpu_start: Starting scheduler on PRO CPU.
I (0) cpu_start: Starting scheduler on APP CPU.
Wake up from timer. Time spent in deep sleep: 20313ms
ULP did 110 temperature measurements in 20313 ms
Initial T=87, latest T=87
Enabling timer wakeup, 20s
Enabling EXT1 wakeup on pins GPIO2, GPIO4
Touch pad #8 average: 2149, wakeup threshold set to 2049.
Touch pad #9 average: 2146, wakeup threshold set to 2046.
Enabling touch pad wakeup
Enabling ULP wakeup
Entering deep sleep
```
