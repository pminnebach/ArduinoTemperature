# Temperature

This project sends the temperature from a (Adafruit) bmp280 sensor to an MQTT server.

## Hardware

- [Adafruit Feather HUZZAH with ESP8266](https://www.adafruit.com/product/2821)
- [Adafruit bmp280](https://www.adafruit.com/product/2651)
- [Adafruit 0.56" 4-Digit 7-Segment Display w/I2C Backpack - Blue](https://www.adafruit.com/product/881)

## Libraries

- [Adafruit_BMP280_Library](https://github.com/adafruit/Adafruit_BMP280_Library)
- [Adafruit_LED_Backpack](https://github.com/adafruit/Adafruit_LED_Backpack)

## Wiring

![Image of wiring](https://raw.githubusercontent.com/pminnebach/ArduinoTemperature/master/sketch_bb.jpg)

## Shoulder, giants

https://funprojects.blog/2018/12/07/rabbitmq-for-iot/
https://microcontrollerslab.com/esp32-static-fix-ip-address/
https://github.com/esp8266/Arduino/issues/3489#issuecomment-454443182
https://github.com/esp8266/Arduino/blob/8a64a1236f447009bd53fec4863aff9bf5428b59/libraries/ESP8266WiFi/examples/EarlyDisableWiFi/EarlyDisableWiFi.ino#L18
