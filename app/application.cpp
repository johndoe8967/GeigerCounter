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
#include <HardwarePWM.h>

#include "../include/SyncNtpDelegate.h"

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
	#define WIFI_SSID "PleaseEnterSSID" // Put you SSID and Password here
	#define WIFI_PWD "PleaseEnterPass"
#endif


uint8_t pwm_pin[1] = { 4 }; // List of pins that you want to connect to pwm
#define INT_PIN 0   // GPIO0

HardwarePWM HW_pwm(pwm_pin, 1);
SyncNTP *syncNTP;

Timer procTimer;

//Geiger Counter Variables
uint32 event_counter;
uint32 actMeasureIntervall;		// last measure intervall in us
uint32 setMeasureIntervall;		// set value for measure intervall in us
bool doMeasure;

void IRAM_ATTR interruptHandler()
{
	event_counter++;
	debugf( "!!!Event");
}

void Loop() {
uint32 actMicros = micros();
	if (!doMeasure) {
		attachInterrupt(INT_PIN, interruptHandler, FALLING);
		actMeasureIntervall = actMicros;
		doMeasure = true;
	} else {
		auto actIntervall = actMicros - actMeasureIntervall;
		if (actIntervall > setMeasureIntervall) {
			detachInterrupt(INT_PIN);
			actMeasureIntervall = actIntervall;
			// send Measurement
			debugf("Events: %ld",event_counter);
			debugf("Interfall: %ld", actMeasureIntervall);
			doMeasure = false;
			event_counter = 0;
		}
	}
}

// Will be called when WiFi station was connected to AP
void connectOk()
{
	syncNTP = new SyncNTP();
}

// Will be called when WiFi station timeout was reached
void connectFail()
{
	debugf("I'm NOT CONNECTED!");
	WifiStation.waitConnection(connectOk, 10, connectFail); // Repeat and check again
}

void init() {
	Serial.begin(SERIAL_BAUD_RATE); // 115200 by default
	Serial.systemDebugOutput(true); // Enable debug output to serial

	// WIFI not needed for demo. So disabling WIFI.
	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD); // Put you SSID and Password here

	WifiAccessPoint.enable(false);

	// Setting PWM values on 8 different pins
	HW_pwm.setPeriod(400);
	HW_pwm.analogWrite(4, HW_pwm.getMaxDuty());

	procTimer.initializeMs(1000,TimerDelegate(&Loop));


	// set timezone hourly difference to UTC
	SystemClock.setTimeZone(2);

	WifiStation.waitConnection(connectOk, 30, connectFail); // We recommend 20+ seconds at start


}
