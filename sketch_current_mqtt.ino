/*
 * ESP8266 + CT Sensor SCT013 + MQTT + Wi-Fi
 * Code for measuring current comes from this link:
 * https://newbiely.com/tutorials/esp8266/esp8266-temperature-sensor
 * Additionally this version has a MQTT and Wi-Fi component to send data
 */

//Libraries
#include <ESP8266WiFi.h>
#include <WiFiClientSecure.h>
#include "EmonLib.h"
#include <MQTTClient.h>
#include <ArduinoJson.h>

//Wi-Fi Credentials
const char *ssid =  "xxxxxxx";   // CHANGE IT AS YOU DESIRE
const char *password =  "xxxxxxx"; // CHANGE IT AS YOU DESIRE

//MQTT Config
const char *MQTT_BROKER_ADRRESS = "192.168.0.148"; //CHANGE TO MQTT BROKER'S ADDRESS
const int MQTT_PORT = 1883;

const char CLIENT_ID[] = "SunMon-ESP8266-001-Energy";
const char MQTT_USERNAME[] = "";                        // CHANGE IT IF REQUIRED, empty if not required
const char MQTT_PASSWORD[] = "";                        // CHANGE IT IF REQUIRED, empty if not required

// The MQTT topics that ESP8266 should publish/subscribe
#define PUBLISH_TOPIC "energy"  // CHANGE IT AS YOU DESIRE
//const char SUBSCRIBE_TOPIC[] = "YOUR-NAME-esp8266-001/loopback";  // CHANGE IT AS YOU DESIRE

// Time between MQTT publications and measurements:
// 5 sec (5000 ms)
// 15 min (900 000 ms)
#define PUBLISH_INTERVAL 15000  

EnergyMonitor SCT013;
int pinSCT = A0; //sensor pin (make sure is analog pin) is connected to A0

WiFiClient network;
MQTTClient mqtt = MQTTClient(256); //Buffer

unsigned long lastPublishTime = 0;

void setup() {
  Serial.begin(9600); // Initialize the Serial to communicate with the Serial Monitor.
  delay(10);

  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  
  // Print ESP8266 Local IP Address
  Serial.print("Connected to WiFi network with IP Address: ");
  Serial.println(WiFi.localIP());

  connectToMQTT();

  SCT013.current(pinSCT,  111.11);

}

void loop() {
  mqtt.loop();
  delay(10);

  if (millis() - lastPublishTime > PUBLISH_INTERVAL) {
    sendToMQTT();
    lastPublishTime = millis();
  }
}

void connectToMQTT() {
  // Connect to the MQTT broker
  mqtt.begin(MQTT_BROKER_ADRRESS, MQTT_PORT, network);

  // Create a handler for incoming messages
  //mqtt.onMessage(messageHandler);

  Serial.print("Trying to connecto to: ");
  Serial.println(MQTT_BROKER_ADRRESS);
  Serial.print("ESP8266 - Connecting to MQTT broker");

  
  while (!mqtt.connect(CLIENT_ID, MQTT_USERNAME, MQTT_PASSWORD)) {
    Serial.print(mqtt.lastError());
    Serial.print(".");
    delay(1000);
  }
  Serial.println();

  if (!mqtt.connected()) {
    Serial.println("ESP8266 - MQTT broker Timeout!");
    return;
  }
  // The following is code to subscribe to a MQTT topic:
  //// Subscribe to a topic, the incoming messages are processed by messageHandler() function
  //if (mqtt.subscribe(SUBSCRIBE_TOPIC))
  //  Serial.print("ESP8266 - Subscribed to the topic: ");
  //else
  //  Serial.print("ESP8266 - Failed to subscribe to the topic: ");
  //
  //Serial.println(SUBSCRIBE_TOPIC);
  //Serial.println("ESP8266  - MQTT broker Connected!");
}

void sendToMQTT() {
  int adcValue = analogRead(A0); // Read raw ADC value
  float Irms_now = SCT013.calcIrms(1480);
  float Irms = Irms + SCT013.calcIrms(1480);
  float I_calib = Irms_now*1.0548-0.0142;  //use calib factor here 
  float power =  power + I_calib * 220;
  

  StaticJsonDocument<200> message;
  message["current"] = Irms;  
  message["power"] = power; //This could me modified so its not required to send power as data, it could be calculated at server
  char messageBuffer[512];
  serializeJson(message, messageBuffer);

  mqtt.publish(PUBLISH_TOPIC, messageBuffer);

  Serial.println("ESP8266 - sent to MQTT:");
  Serial.print("- topic: ");
  Serial.println(PUBLISH_TOPIC);
  Serial.print("- payload:");
  Serial.println(messageBuffer);
}
