/*
 * File: geigr counter
 * Original Author: https://github.com/johndoe8967
 *
 * a geiger counter application with IoT connection to ThingSpeak and Radmon
 *
 * detection of radioactive decay events on interrupt pin 0
 * 2 modes: stationary (wifi client) mobile (wifi SoftAP) depending on pin 2
 *
 * enhanced to measure air quality
 *
 * basic settings are changeable as telnet commands
 *
 * Created on June 5, 2016
 */
#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <SmingCore/Debug.h>

#include <HardwarePWM.h>

#include "../include/CommandClass.h"
#include "../include/sendData.h"
#include "../include/SyncNtpDelegate.h"
#include "../include/AppSettings.h"
#include "../include/SDS011.h"

#define INT_PIN 0   // GPIO0 to detect radiation (has to be true at boot)
#define MODE_PIN 2	// GPIO2 to change mode (has to be true at boot)

enum {stationary, mobile} mode;

SyncNTP *syncNTP;
CommandClass commands;

ApplicationSettingsStorage AppSettings;

Timer measureTimer;
Timer backgroundTimer;

//Geiger Counter Variables
uint32 event_counter;
uint32 actMeasureInterval = 0;				// last measure intervall in us
uint32 setMeasureInterval = 60000000;		// set value for measure intervall in us
float doseRatio;
bool online=true;

SDS011 feinStaub;							// particle sensor
HardwareSerial feinStaubInterface(0);		// serial interface


void IRAM_ATTR interruptHandler()
{
	event_counter++;						// count radiation event
}

void taskMeasure() {						// cyclic measurement task 100ms
	uint32 actMicros = micros();
	auto actInterval = actMicros - actMeasureInterval;
	bool stopMeasure = false;

	if (setMeasureInterval == 0) {			// if no measure interval is set then stop after 100 events or 15 seconds
		if ((event_counter >= 100) && (actInterval > 15000000)) {
			stopMeasure = true;
		}
	} else {
		stopMeasure = true;
	}

	if (stopMeasure) {
		actMeasureInterval = actInterval;
		// send Measurement
		Debug.printf("Events: %ld ",event_counter);
		Debug.printf("Interfall: %ld\r\n", actMeasureInterval);

		auto events = event_counter;
		event_counter = 0;
		sendData(events, actMeasureInterval, online);
		actMeasureInterval = actMicros;

		if (setMeasureInterval==0) {		// is no measure interval is set check periodically with 100ms
			measureTimer.setIntervalMs(100);
		} else {
			measureTimer.setIntervalUs(setMeasureInterval-(micros()-actMicros));
		}
	}
}


// Will be called when WiFi station was connected to AP
void connectOk()
{
	if (!syncNTP) syncNTP = new SyncNTP();

}

// Will be called when WiFi station timeout was reached
void connectFail()
{
	debugf("I'm NOT CONNECTED!");
	WifiStation.waitConnection(connectOk, 10, connectFail); // Repeat and check again
}

void taskBackground() {
int dustDelay = 0;
	dustDelay++;						// increase every 5 seconds
	if (dustDelay%12==0) {				// wake up sensor every 60s
		feinStaub.wakeup();
	}
	if (dustDelay%12==1) {				// wait 5s after wakeup to receive data and sleep again
		float p25;
		float p10;
		if (feinStaub.read(p25,p10)) {	// read and send measurement
			sendDust(p25,p10);
		}
		feinStaub.sleep();
	}

	switch (mode) {
	case mobile:
		if (digitalRead(MODE_PIN)) {		// switch to stationary mode
			mode = stationary;
			WifiAccessPoint.enable(false);

			WifiStation.enable(true);
			WifiStation.config(AppSettings.WLANSSID,AppSettings.WLANPWD);
			WifiStation.waitConnection(connectOk, 30, connectFail); // We recommend 20+ seconds at start

		}
		break;
	case stationary:
		if (syncNTP) {
			online = syncNTP->valid;		// check for valid network time
		}

		if (!digitalRead(MODE_PIN)) {		// switch to mobile mode
			mode = mobile;
			online = false;

			delete(syncNTP);

			WifiStation.disconnect();
			WifiStation.enable(false);

			WifiAccessPoint.config("RadMon","RadMon", AUTH_OPEN);
			WifiAccessPoint.enable(true);
		}
		break;
	default:
		mode = mobile;
		break;
	}

}

void setTime(unsigned int time) {
	if (time <= 3600) {
		uint32 timeus = time*1000000;
		Debug.printf("measuretime: %ld\r\n", timeus);
		setMeasureInterval = timeus;
	}
}

void init() {
//	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
//	Serial.systemDebugOutput(false); // Enable debug output to serial

	delayMilliseconds(1000);

	spiffs_mount(); // Mount file system, in order to work with files


	commandHandler.registerSystemCommands();
	commands.init(SetTimeDelegate(&setTime));

	AppSettings.load();

	mode = mobile;

	WifiStation.enable(false);
	WifiStation.config(AppSettings.WLANSSID,AppSettings.WLANPWD);

	WifiAccessPoint.enable(false);
	WifiAccessPoint.config("RadMon","RadMon", AUTH_WPA_PSK);

	pinMode(INT_PIN, INPUT);
	pinMode(MODE_PIN,INPUT);


	// init timer for first start after 100ms
	measureTimer.initializeMs(100,TimerDelegate(&taskMeasure)).start();
	backgroundTimer.initializeMs(5000,TimerDelegate(&taskBackground)).start();

	// set timezone hourly difference to UTC
	SystemClock.setTimeZone(2);
	attachInterrupt(INT_PIN, interruptHandler, RISING);

	feinStaub.begin(&feinStaubInterface);

}
