#include <Wire.h>
#include <SPI.h>
#include <ESP8266WiFi.h>          // WiFi
#include <PubSubClient.h>         // MQTT
#include <Adafruit_Sensor.h>      // bmp280
#include <Adafruit_BMP280.h>      // bmp280
#include <Adafruit_GFX.h>         // Seven segment
#include "Adafruit_LEDBackpack.h" // Seven segment
#include <WiFiUdp.h>              // NTP
#include <NTPClient.h>            // NTP
#include "temperature_config.h"   // Private configuration file

const char *ssid = WIFI_SSID;
const char *password = WIFI_PASSWORD;
const char *mqtt_server = MQTT_SERVER;
const char *mqtt_user = MQTT_USER;
const char *mqtt_pass = MQTT_PASSWORD;
unsigned long lastMsg = 0;

WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

// Static IP Configuration
IPAddress staticIP = IP_ADDRESS;
IPAddress gateway = IP_GATEWAY;
IPAddress subnet = IP_SUBNET;
IPAddress dns = IP_DNS;

// Device hostname configuration
const char *deviceName = DEVICE_NAME;

Adafruit_7segment matrix = Adafruit_7segment();
Adafruit_BMP280 bmp; // I2C

void setup_wifi()
{
  WiFi.disconnect();  //Prevent connecting to wifi based on previous configuration  
  WiFi.hostname(deviceName);
  if (!WiFi.config(staticIP, gateway, subnet, dns)) {
    Serial.println("STA Failed to configure");
  }
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }  
  
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  Serial.println();
  // WiFi.printDiag(Serial);
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

void preinit() {
	/*
	 * Global WiFi constructors are not called yet
	 * (global class instances like WiFi, Serial... are not yet initialized)..
	 * No global object methods or C++ exceptions can be called in here!
	 * The below is a static class method, which is similar to a function, so it's ok.
	 */
  ESP8266WiFiClass::preinitWiFiOff();
}

void setup() 
{
  /* 
   *  Prevent the device from connecting to any wifi
   *  before we're able to set the static IP configuration.
   */
  WiFi.setAutoConnect(false);

  // Enable or disable debug according to your needs.
  Serial.setDebugOutput(false);
  
#ifndef __AVR_ATtiny85__
  Serial.begin(115200);
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

  Serial.println("setup() NTP timeClient.begin().");
  timeClient.begin();
}

void loop() 
{
  if (!client.connected())
  {
    reconnect();
  }
  client.loop();

  timeClient.update();

  char msg[30];
  float temperature,pressure;
  long epoch;
  
  temperature = bmp.readTemperature();
  pressure = bmp.readPressure();
  epoch = timeClient.getEpochTime();

  Serial.print(F("Epoch       = "));
  Serial.print(epoch);
  Serial.println();
 
  Serial.print(F("Temperature = "));
  Serial.print(temperature);
  Serial.println(" *C");

  Serial.print(F("Pressure    = "));
  Serial.print(pressure/100);
  Serial.println(" hPa");

  Serial.println();

  // print a floating point 
  matrix.print(temperature);
  matrix.writeDisplay();

  unsigned long now = millis();
  if (now - lastMsg > 60000)
  {
    lastMsg = now;

    //sprintf(msg, "%f", bmp.readTemperature());
    sprintf(msg, "%ld;%.2f;%.2f",epoch,temperature,(pressure/100));
    client.publish("mq2_mqtt", msg);
    Serial.print(F("Message = "));
    Serial.print(msg);
    Serial.println();
  }
  
  delay(5000);
}
