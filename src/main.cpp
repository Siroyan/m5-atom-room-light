#include <Arduino.h>
#include <WiFi.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include "Credentials.h"
#include "M5Atom.h"

const char* ssid = WIFI_SSID;
const char* password = WIFI_PW;
WiFiServer server(80);

const uint16_t kIrLed = G33;
IRsend irsend(kIrLed);

uint16_t rawData[135] = {4740, 2546, 644, 778, 666, 1722, 692, 1750, 644, 850, 666, 1722, 694, 
						1748, 646, 776, 666, 856, 670, 780, 696, 1716, 668, 1720, 674, 848, 700,
						1716, 668, 1720, 696, 1716, 668, 854, 672, 778, 666, 784, 668, 782, 692,
						848, 668, 782, 670, 780, 694, 1714, 670, 850, 666, 786, 668, 782, 672, 778,
						696, 846, 670, 806, 646, 776, 668, 782, 670, 740, 670, 18174, 4746, 2544,
						646, 778, 696, 1714, 670, 1718, 666, 856, 702, 1712, 672, 1716, 668, 782,
						702, 840, 666, 784, 668, 1720, 696, 1718, 666, 856, 670, 1718, 698, 1718,
						666, 1722, 674, 848, 698, 772, 670, 780, 674, 776, 666, 856, 672, 806, 668,
						774, 668, 1720, 676, 874, 674, 796, 646, 804, 638, 812, 642, 880, 646, 804,
						670, 798, 644, 806, 648, 764, 638};

void setup() {
	irsend.begin();

	// シリアル通信
	Serial.begin(115200);
	delay(1000);

	// Wi-Fi
	WiFi.begin(ssid, password);
	while (WiFi.status() != WL_CONNECTED);
	Serial.println(WiFi.localIP());

	server.begin();

	M5.begin(false, false, true);
}

void loop() {
	WiFiClient client = server.available();
	if (client) {
		String currentLine = "";
		while (client.connected()) {
			if (client.available()) {
				char c = client.read();
				if (c == '\n') {
					if (currentLine.length() == 0) {
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println();
						client.print("<a href=\"/switch\">here</a>");
						client.println();
						break;
					} else {
						currentLine = "";
					}
				} else if (c != '\r') {
					currentLine += c;
				}
				if (currentLine.endsWith("GET /switch")) {
					Serial.println("pushed");
					for(int x = 0; x < 5; x++) {
						for (int y = 0; y < 5; y++) {
							M5.dis.drawpix(x, y, 0xFFFFFF);
						}
					}
					irsend.sendRaw(rawData, 135, 38);
					delay(250);
					for(int x = 0; x < 5; x++) {
						for (int y = 0; y < 5; y++) {
							M5.dis.drawpix(x, y, 0x000000);
						}
					}
					irsend.sendRaw(rawData, 135, 38);
				}
			} 
		}
		client.stop();
		Serial.println("Disconnected");
	}
}