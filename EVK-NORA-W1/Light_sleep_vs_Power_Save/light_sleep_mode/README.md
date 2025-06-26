# Current consumption on NORA-W1 using light-sleep mode

## Hardware

NORA-W1 (ESP32-S3)

## ESP-IDF version

5.1.2

## Simulation description

- Light-sleep mode
- References:
    - [Wi-Fi/Bluetooth and Sleep Modes](https://docs.espressif.com/projects/esp-idf/en/v5.1.2/esp32/api-reference/system/sleep_modes.html#wi-fi-bluetooth-and-sleep-modes)
    - Espressif GitHub example: [system -> light_sleep](https://github.com/espressif/esp-idf/tree/b3f7e2c8a4d354df8ef8558ea7caddc07283a57b/examples/system/light_sleep#light-sleep-example)
- NORA-W1 transmits data periodically via TCP link:
    - 300bytes each 30s and
- TCP Server was created on the [TCP Debugger app](https://apps.microsoft.com/detail/9NWV1TCX232T?hl=en-us&gl=US).
    - Also, it was tested using the YAT software.

