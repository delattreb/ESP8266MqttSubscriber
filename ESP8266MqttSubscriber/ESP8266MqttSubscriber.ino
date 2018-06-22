#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include "var.h"

//MQTT
char mqttServer[] = IP_SERVER;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);

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
	String msg = "";
	for (int i = 0; i < length; i++)
		msg.concat(char(payload[i]));
#ifdef DEBUG
	Serial.print("Value: ");
	Serial.println(msg);
#endif
	int ventilation = msg.toInt();

	if (ventilation == 1) {
#ifdef DEBUG
		Serial.println("GPIO0: On");
#endif
		digitalWrite(GPIO_0, LOW);
	}
	if (ventilation == 0)
	{
#ifdef DEBUG
		Serial.println("GPIO0: Off");
#endif#endif
		digitalWrite(GPIO_0, HIGH);
	}
}

//
// reconnect
//
void reconnect() {
	// Connect to MQTT
	if (!mqttClient.connected()) {
#ifdef INFO
		Serial.println("Attempting MQTT connection...");
#endif
		while (!mqttClient.connected()) {
#ifdef INFO
			Serial.print(".");
#endif
			mqttClient.connect(NETWORKNAME);
			delay(ATTENPTING);
		}
#ifdef INFO
		Serial.println("");
		Serial.println("Connected");
#endif
		mqttClient.subscribe(TOPIC);
	}
}

//
// setup
//
void setup()
{
	Serial.begin(SERIALBAUDS);
	while (!Serial) {
		;
	}
#ifdef INFO
	delay(1500);
	Serial.print("Core version: ");
	Serial.println(ESP.getCoreVersion());
	Serial.print("Sdk version: ");
	Serial.println(ESP.getSdkVersion());
#endif 
	WiFiManager wifiManager;
	//Reset setting
	//wifiManager.resetSettings();

	wifiManager.setAPStaticIPConfig(IPAddress(IPLOWA, IPLOWB, IPLOWC, IPLOWD), IPAddress(IPHIGHA, IPHIGHB, IPHIGHC, IPHIGHD), IPAddress(255, 255, 255, 0));
	if (!wifiManager.autoConnect(NETWORKNAME)) {
		Serial.println("Failed to connect");
		delay(1000);
		ESP.reset();
		delay(5000);
	}

	//MQTT configuration
	mqttClient.setServer(mqttServer, MQTTPORT);
	mqttClient.setCallback(callback);
	reconnect();

	//ESP8266 Configuration
	pinMode(GPIO_0, OUTPUT);
	digitalWrite(GPIO_0, HIGH);
	
	//TODO: Test sleep for 30s
	//ESP.deepSleep(30e6);  
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
