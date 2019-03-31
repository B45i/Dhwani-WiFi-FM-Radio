#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <Wire.h>
#include "TEA5767.h"


const char KEY[] = "c3dac95421ff4c92a041501f0dafb42d";
const char SSID[] = "EvilCorp";
const char PASSWORD[] = "helloworld";

const int PIN_SCL = 0;
const int PIN_SDA = 3;

TEA5767 Radio;

int search_mode = 0;
int g_muted = false;
int search_direction;
unsigned char buf[5];

double currentFreq = 92.7;
int signalLevel;
int stereo;


BLYNK_WRITE(V5)
{
	String action = param.asStr();

	if (action == "play") {
		setMute(true);
		// Serial.println("Play");
	}
	if (action == "stop") {
		setMute(false);
		// Serial.println("Stop");
	}
	if (action == "next") {
		searchUp();
		// Serial.println("next");
	}
	if (action == "prev") {
		searchDown();
		// Serial.println("prev");
	}

	//Blynk.setProperty(V5, "label", action);
	Serial.print(action);
}

void info() {
	if (Radio.read_status(buf) == 1) {
		currentFreq = floor(Radio.frequency_available(buf) / 100000 + .5) / 10;
		signalLevel = Radio.signal_level(buf);
		stereo = Radio.stereo(buf);

		// Serial.print("Tuned to : ");
		// Serial.print(currentFreq);
		Blynk.virtualWrite(V0, currentFreq);
		// Serial.print("\t Signal strength: ");
		// Serial.println(signalLevel);		
		Blynk.virtualWrite(V1, signalLevel);

		// if (stereo)
		// 	Serial.println("(stereo)\n");
		// else
		// 	Serial.println("(mono)\n");

		if (g_muted)
			Serial.println("(muted)\n");
	}
	else {
		Serial.println("Radio Not Connected !\n");
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
	Serial.println("Searching down ..\n");
}

void searchUp() {
	search_mode = 1;
	search_direction = TEA5767_SEARCH_DIR_UP;
	Radio.search_up(buf);
	Serial.println("Searching up ..\n");
}

void tuneTo(double freq) {
	Serial.print("Tuning to: ");
	Serial.println(freq);
	Radio.set_frequency(freq);
}

void setup() {
	Serial.begin(115200);
	Blynk.begin(KEY, SSID, PASSWORD);
	Wire.begin();
	Radio.init();
	Radio.set_frequency(currentFreq);
}

void loop() {
	Blynk.run();

	// if (search_mode == 1) {
	// 	if (Radio.read_status(buf) == 1) {
	// 		if (Radio.process_search(buf, search_direction) == 1) {
	// 			search_mode = 0;
	// 		}
	// 	}

	// }
	
	info();
}