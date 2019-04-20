#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include "TEA5767.h"


const char KEY[] = "YOUR_BLYNK_KEY";
const char SSID[] = "YOUR_WIFI_SSID";
const char PASSWORD[] = "YOUR_WIFI_PASSWORD";

const int PIN_SDA = 0; // D3;
const int PIN_SCL = 3; // Rx;

TEA5767 Radio;

int search_mode = 0;
int g_muted = false;
int search_direction;
unsigned char buf[5];

double currentFreq = 92.7;
int signalLevel;


BLYNK_WRITE(V5) {
	String action = param.asStr();
	if (action == "play") 	   { setMute(true);  }
	else if (action == "stop") { setMute(false); }
	else if (action == "next") { searchUp(); 	 }
	else if (action == "prev") { searchDown();	 }
}

BLYNK_WRITE(V2) {
	tuneTo(param.asFloat());
}

void info() {
	if (Radio.read_status(buf) == 1) {
		currentFreq = floor(Radio.frequency_available(buf) / 100000 + .5) / 10;
		signalLevel = Radio.signal_level(buf);

		Blynk.virtualWrite(V0, currentFreq);
		Blynk.virtualWrite(V1, signalLevel);
		// Serial.println(signalLevel);
	}
	else {
		//// Serial.println("Radio Not Connected !\n");
		Blynk.virtualWrite(V0, "Radio Not Connected !");
	}
}

void setMute(bool state) {
	if (state) {
		Radio.mute();
	}
	else {
		if (Radio.read_status(buf) == 1) {
			double currentFreq = floor(Radio.frequency_available(buf) / 100000 + .5) / 10;
			Radio.set_frequency(currentFreq);
		}
	}
	g_muted = state;
}

void searchDown() {
	search_mode = 1;
	search_direction = TEA5767_SEARCH_DIR_DOWN;
	Radio.search_down(buf);
	// Serial.println("Searching down ..\n");
}

void searchUp() {
	search_mode = 1;
	search_direction = TEA5767_SEARCH_DIR_UP;
	Radio.search_up(buf);
	// Serial.println("Searching up ..\n");
}

void tuneTo(double freq) {
	// Serial.print("Tuning to: ");
	// Serial.println(freq);
	Radio.set_frequency(freq);
}

void setup() {
	// Serial.begin(115200);
	Blynk.begin(KEY, SSID, PASSWORD);
	Wire.begin(PIN_SDA, PIN_SCL);
	Radio.init();
	Radio.set_frequency(currentFreq);
}

void loop() {
	Blynk.run();
	info();
}
