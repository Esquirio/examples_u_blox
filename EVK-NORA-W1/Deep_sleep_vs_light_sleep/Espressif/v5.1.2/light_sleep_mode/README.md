|                   |          |
| ----------------- | -------- |
| Supported Targets | ESP32-S3 |
|  esp-idf version  |   5.1.1  |

# Light Sleep Mode - NORA-W1

This code is based on [ESP example ligth_sleep](examples/system/light_sleep) and illustrates usage of light sleep mode for current consuption measurement. Unlike deep sleep mode, light sleep preserves the state of the memory, CPU, and peripherals. Execution of code on both CPUs is stopped when `esp_light_sleep_start()` function is called. When the chip exits light sleep mode, execution continues at the point where it was stopped, and `esp_light_sleep_start()` function returns.

The example enables the following wakeup source:

- Timer: wake up the chip in 2 seconds

## How to Use Example

### Hardware Required

This example can be used with any ESP32-S3 development board (e.g.: EVK NORA-W1). Most boards have a button attached to GPIO0, often labelled `BOOT`. If the board does not have such button, an external button can be connected, along with a 10k pull-up resistor, and a 100nF capacitor to ground for debouncing.

### Build and Flash

Set the device target:
```
idf.py set-target esp32-s3
```

Build the project and flash it to the board, then run monitor tool to view serial output:

```
idf.py -p PORT flash monitor
```

(Replace PORT with the name of the serial port to use.)

(To exit the serial monitor, type ``Ctrl-]``.)

See the Espressif Getting Started Guide for full steps to configure and use ESP-IDF to build projects.

## Example Output

### Wake-up by Timer

If do nothing to the example, the chip will wake-up every 2000 ms by timer, and fall into light sleep again after print some logs. We can see the wake-up reason is `timer` in this case.

```
I (350) Light Sleep: Entering light-sleep mode
I (350) Light Sleep: Entering light-sleep mode
I (360) Light Sleep: Entering light-sleep mode
I (360) Light Sleep: Entering light-sleep mode
I (370) Light Sleep: Entering light-sleep mode
I (370) Light Sleep: Entering light-sleep mode
I (380) Light Sleep: Entering light-sleep mode
...
