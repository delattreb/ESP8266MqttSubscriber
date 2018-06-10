#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>
#include <PubSubClient.h>
#include "var.h"

char mqttServer[] = IP_SERVER;
WiFiClient wifiClient;
PubSubClient client(wifiClient);
String data;
const int CHAR = 48;
static unsigned long previousMillis = 0;
unsigned long currentMillis;

//
// reconnect
//
void callback(char* topic, byte* payload, unsigned int length) {
#ifdef INFO
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (int i = 0; i < length; i++) {
		Serial.print((char)payload[i]);
	}
	Serial.println();
#endif
}

//
// reconnect
//
void reconnect() {
	// Connect to MQTT
	if (!client.connected()) {
#ifdef INFO
		Serial.println("Attempting MQTT connection...");
#endif
		while (!client.connected()) {
#ifdef INFO
			Serial.print(".");
#endif
			client.connect(NETWORKNAME);
			delay(ATTENPTING);
		}
#ifdef INFO
		Serial.println("");
		Serial.println("Connected");
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
	client.setServer(mqttServer, MQTTPORT);
	client.setCallback(callback);
	reconnect();
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
	if (client.connected())
		client.loop();
	//code
}

//
// sendMQTT
//
void sendMQTT(char sensor, String temp, String hum) {
	// Send payload
	String strT = "iot:t";
	String strH = "iot:h";
	char attributest[100];
	char attributesh[100];
	temp.toCharArray(attributest, 100);
	hum.toCharArray(attributesh, 100);
	if (client.connected()) {
#ifdef DEBUG
		Serial.println("Before send to MQTT broker:");
		Serial.println(temp);
		Serial.println(hum);
		Serial.println(sensor);
		Serial.println(attributest);
		Serial.println(attributesh);
#endif 	
		strT.concat(sensor);
		strH.concat(sensor);
		client.publish(strT.c_str(), attributest);
		client.publish(strH.c_str(), attributesh);
	}
}