#include <ArduinoJson.h>
#include <ESP8266WiFi.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include "var.h"
#include "mqtt.h"

//MQTT
char nodeServer[] = IP_SERVER;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
char json[JSONBUFFER];

//
// reconnect
//
void callback(char* topic, byte* payload, unsigned int length) {
#ifdef DEBUG
	Serial.print("Message: ");
	Serial.println(topic);
	Serial.print("Length: ");
	Serial.println(length);
#endif
	if (strcmp(topic, TOPIC_VENTILATION) == 0) {
		for (int i = 0; i < length; i++)
			json[i] = (char)payload[i];

		StaticJsonBuffer<JSONBUFFER> jsonBuffer;
		JsonObject& root = jsonBuffer.parseObject(json);
		if (!root.success()) {
#ifdef DEBUG
			Serial.println("parse json failed");
#endif
			return;
		}
		const char* value = root["value"];
#ifdef DEBUG
		Serial.print("Value: ");
		Serial.println(value);
#endif
		if (strcmp(value, "0") == 0)
			digitalWrite(GPIO_0, HIGH);
		if (strcmp(value, "1") == 0)
			digitalWrite(GPIO_0, LOW);
	}
}

//
// reconnect
//
void reconnect() {
	// Connect to MQTT
	if (!mqttClient.connected())
	{
#ifdef INFO
		Serial.println("Attempting MQTT connection...");
#endif
		while (!mqttClient.connected())
		{
#ifdef INFO
			Serial.print(mqttClient.state());
#endif
			mqttClient.connect(ESP_NAME);
			delay(ATTENPTING);
		}
#ifdef INFO
		Serial.println("");
		Serial.println("MQTT connected");
#endif
		mqttClient.subscribe(TOPIC_VENTILATION);
	}
}

//
// setup
//
void setup()
{
	Serial.begin(SERIALBAUDS);
	while (!Serial)  continue;

#ifdef INFO
	delay(1500);
	Serial.print("Core version: ");
	Serial.println(ESP.getCoreVersion());
	Serial.print("Sdk version: ");
	Serial.println(ESP.getSdkVersion());
	Serial.print("MAC Address: ");
	Serial.println(WiFi.macAddress());
#endif
	WiFiManager wifiManager;
	//Reset setting
	//wifiManager.resetSettings();

	wifiManager.setAPStaticIPConfig(IPAddress(IPLOWA, IPLOWB, IPLOWC, IPLOWD), IPAddress(IPHIGHA, IPHIGHB, IPHIGHC, IPHIGHD), IPAddress(255, 255, 255, 0));
	wifiManager.setDebugOutput(false);
	if (!wifiManager.autoConnect(ESP_NAME))
	{
#ifdef DEBUG
		Serial.println("Failed to connect");
#endif
		delay(1000);
		ESP.reset();
		delay(5000);
	}

	mqttClient.setServer(nodeServer, MQTTPORT);
	mqttClient.setCallback(callback);
	reconnect();

	//ESP8266 Configuration
	pinMode(GPIO_0, OUTPUT);
	digitalWrite(GPIO_0, HIGH);
}

//
// loop
//
void loop()
{
	reconnect();

	if (mqttClient.connected())
		mqttClient.loop();
}
