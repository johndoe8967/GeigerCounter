/*
 * File: Esp SDK Hardware PWM demo
 * Original Author: https://github.com/hrsavla
 *
 * This HardwarePWM library enables Sming framework user to use ESP SDK PWM API
 * Period of PWM is fixed to 1000ms / Frequency = 1khz
 * Duty at 100% = 22222. Duty at 0% = 0
 * You can use function setPeriod() to change frequency/period.
 * Calculate the Duty as per the formulae give in ESP8266 SDK
 * Duty = (Period *1000)/45
 *
 * PWM can be generated on upto 8 pins (ie All pins except pin 16)
 * Created on August 17, 2015, 2:27 PM
 */
#include <user_config.h>
#include <SmingCore/SmingCore.h>
#include <SmingCore/Debug.h>

#include <HardwarePWM.h>

#include "../include/CommandClass.h"
#include "../include/sendData.h"
#include "../include/SyncNtpDelegate.h"
#include "../include/AppSettings.h"

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
	#define WIFI_SSID "PleaseEnterSSID" // Put you SSID and Password here
	#define WIFI_PWD "PleaseEnterPass"
#endif

#define INT_PIN 2   // GPIO0
#define MODE_PIN 0	// GPIO2

enum {stationary, mobile} mode;

#ifdef USEPWM
#define PWM_PIN 2	// GPIO2
uint8_t pwm_pin[1] = { PWM_PIN }; // List of pins that you want to connect to pwm
HardwarePWM HW_pwm(pwm_pin, 1);
#endif

SyncNTP *syncNTP;
CommandClass commands;

ApplicationSettingsStorage AppSettings;

Timer measureTimer;
Timer backgroundTimer;

//Geiger Counter Variables
uint32 event_counter;
uint32 actMeasureIntervall = 0;				// last measure intervall in us
uint32 setMeasureIntervall = 60000000;		// set value for measure intervall in us
bool doMeasure;
float doseRatio;
bool online=true;


void IRAM_ATTR interruptHandler()
{
	event_counter++;
}

void Loop() {
	uint32 actMicros = micros();
	auto actIntervall = actMicros - actMeasureIntervall;
	bool stopMeasure= false;

	if (setMeasureIntervall == 0) {
		if ((event_counter >= 100) && (actIntervall > 15000000)) {
			stopMeasure = true;
		}
	} else {
		stopMeasure = true;
	}

	if (stopMeasure) {
//		detachInterrupt(INT_PIN);
		actMeasureIntervall = actIntervall;
		// send Measurement
		Debug.printf("Events: %ld ",event_counter);
		Debug.printf("Interfall: %ld\r\n", actMeasureIntervall);

		auto events = event_counter;
		event_counter = 0;
		sendData(events, actMeasureIntervall, online);
		doMeasure = false;
//		attachInterrupt(INT_PIN, interruptHandler, RISING);
		actMeasureIntervall = actMicros;
		doMeasure = true;
		Debug.printf("start measure\r\n");
		if (setMeasureIntervall==0) {
			measureTimer.setIntervalMs(100);
		} else {
			measureTimer.setIntervalUs(setMeasureIntervall-(micros()-actMicros));
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

void background() {
	switch (mode) {
	case mobile:
		if (digitalRead(MODE_PIN)) {
			mode = stationary;
			WifiAccessPoint.enable(false);

			WifiStation.enable(true);
			debugf("SSID: %s", AppSettings.WLANSSID.c_str());
			debugf("PWD: %s", AppSettings.WLANPWD.c_str());
			WifiStation.config(AppSettings.WLANSSID,AppSettings.WLANPWD);
			WifiStation.waitConnection(connectOk, 30, connectFail); // We recommend 20+ seconds at start

		}
		break;
	case stationary:
		if (syncNTP) {
			online = syncNTP->valid;
		}

		if (!digitalRead(MODE_PIN)) {
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


void setPWM(unsigned int duty) {
#ifdef USEPWM
	if (duty <= 100) {
		auto ontime = duty*HW_pwm.getMaxDuty()/100;
		Debug.printf("pwm ontime: %d\r\n",ontime);
		HW_pwm.analogWrite(PWM_PIN, ontime);

	}
#endif
}
void setTime(unsigned int time) {
	if (time <= 3600) {
		uint32 timeus = time*1000000;
		Debug.printf("measuretime: %ld\r\n", timeus);
		setMeasureIntervall = timeus;
	}
}

void init() {
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(false); // Enable debug output to serial

	delayMilliseconds(1000);

	spiffs_mount(); // Mount file system, in order to work with files


	commandHandler.registerSystemCommands();
	commands.init(SetPWMDelegate(&setPWM),SetTimeDelegate(&setTime));

	AppSettings.load();

	mode = mobile;

	WifiStation.enable(false);
	WifiStation.config(AppSettings.WLANSSID,AppSettings.WLANPWD);

	WifiAccessPoint.enable(false);
	WifiAccessPoint.config("RadMon","RadMon", AUTH_WPA_PSK);

	pinMode(INT_PIN, INPUT);
	pinMode(MODE_PIN,INPUT);

	// Setting PWM period to 2,5kHz
#ifdef USEPWM
	HW_pwm.setPeriod(1000);
#endif

	// init timer for first start after 100ms
	measureTimer.initializeMs(100,TimerDelegate(&Loop)).start();
	backgroundTimer.initializeMs(5000,TimerDelegate(&background)).start();

	// set timezone hourly difference to UTC
	SystemClock.setTimeZone(2);
	attachInterrupt(INT_PIN, interruptHandler, RISING);

}
