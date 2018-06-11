#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include "var.h"

char mqttServer[] = IP_SERVER;
WiFiClient wifiClient;
PubSubClient mqttClient(wifiClient);
String data;
const int CHAR = 48;
static unsigned long previousMillis = 0;
unsigned long currentMillis;

//
// reconnect
//
void callback(char* topic, byte* payload, unsigned int length) {
#ifdef DEBUG
	Serial.print("Message: ");
	Serial.println(topic);
	Serial.print("Value: ");
	Serial.println((char*)payload);
#endif
	String msg = (char*)payload;
	int hum = msg.toInt();

	if (hum >= 72) {
#ifdef DEBUG
		Serial.println("GPIO0: On");
#endif
		digitalWrite(GPIO_0, HIGH);
	}
	else
	{
#ifdef DEBUG
		Serial.println("GPIO0: Off");
#endif#endif
		digitalWrite(GPIO_0, LOW);
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
		mqttClient.subscribe(TOPIC);
#endif
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
	mqttClient.subscribe(TOPIC);

	//ESP8266 Configuration
	pinMode(GPIO_0, OUTPUT);
	digitalWrite(0, LOW);
}

//
// loop
//
void loop()
{
	currentMillis = millis();
	reconnect();
	if (currentMillis - previousMillis >= DB_FREQUENCY) {
		previousMillis = currentMillis;
	}
	if (mqttClient.connected())
		mqttClient.loop();
}
