#include <ArduinoJson.h>
#include "mqtt.h"
#include "var.h"

//********************************
// decodeTopicVentilation
// Description:
//********************************
void decodeTopicVentilation(char json[])
{
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


