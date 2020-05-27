# MQTT Logger Sample Code

## Required
- M5StickC board []
- Arduino IDE development environment
  - Installation: https://www.arduino.cc/en/Main/Software
  - install ESP32 boards in Arduino IDE: https://www.instructables.com/id/Getting-Started-With-ESP32-Installing-ESP32-Boards/

## Installation
- clone this repository
- copy **settingsexample.h** to **settings.h** to start
- fill in wifi and mqtt settings in **settings.h**
- in IDE:
  - Tools > Board -> M5Stick-C
  - Tools > PArtition Scheme -> NO OTA (Large APP)
- compile & flash to the M5Stick

## Handy
- HiveMQ has a free MQTT server @ broker.hivemq.com / 1883
- use a MQTT client to test
  - publish to sensor /
    - send QR code with QR060HELLOWORLD
    - change channel with CH04
  - subscribe on sensor
  

## References
- MQTT pubsubclient library: https://pubsubclient.knolleary.net/api#state
- m5StickC board info:
- M5StickC Arduino library: https://docs.m5stack.com/#/en/arduino/arduino_api
- MQTTfx client: https://mqttfx.jensd.de/


