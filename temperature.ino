#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <Adafruit_GFX.h>
#include "Adafruit_LEDBackpack.h"
#include "iot_config.h" // Include custom configuration

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *mqtt_server = MQTT_SERVER;
const char *mqtt_user = MQTT_USER;
const char *mqtt_pass = MQTT_PASSWORD;

WiFiClient espClient;
PubSubClient client(espClient);

// Hardware
Adafruit_7segment matrix = Adafruit_7segment();
Adafruit_BMP280 bmp; // I2C

unsigned long lastMsg = 0;

void setup_wifi()
{
  // Connecting to a WiFi network
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void setup_sensor()
{
  Serial.println("setup_sensor() bmp280 sensor.");
  Serial.println(F("BMP280 test"));
  if (!bmp.begin())
  {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1)
      ;
  }
}

void reconnect()
{
  // Loop until we're reconnected
  Serial.println("In reconnect...");
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("esp8266_01", mqtt_user, mqtt_pass))
    {
      Serial.println("connected");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}

void setup() {
#ifndef __AVR_ATtiny85__
  Serial.begin(9600);
  delay(100);
  Serial.println("7 Segment Backpack Test");
#endif
  Serial.println("setup() matrix.");
  matrix.begin(0x70);

  Serial.println("setup() wifi.");
  setup_wifi();

  Serial.println("setup() MQTT.");
  client.setServer(mqtt_server, 1883);

  Serial.println("setup() bmp280 sensor.");
  setup_sensor();
}

void loop() {
  char msg[8];
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  float temperature;
  String pressure; 
  
  temperature = bmp.readTemperature();
  pressure = bmp.readPressure();
 
  Serial.print(F("Temperature = "));
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print(F("Pressure = "));
  Serial.print(pressure);
  Serial.println(" Pa");
  
  Serial.println();

  // print a floating point 
  matrix.print(temperature);
  matrix.writeDisplay();

  unsigned long now = millis();
  if (now - lastMsg > 60000)
  {
    lastMsg = now;

    sprintf(msg, "%f", bmp.readTemperature());
    client.publish("mq2_mqtt", msg);
    Serial.print(F("Temperature = "));
    Serial.print(msg);
    Serial.println(" *C");
  }
  
  delay(5000);
}
