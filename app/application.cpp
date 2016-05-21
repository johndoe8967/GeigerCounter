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
#include <SmingCore/Network/TelnetServer.h>
#include <SmingCore/Debug.h>

#include <HardwarePWM.h>

#include "../include/sendData.h"
#include "../include/SetPWMCmd.h"
#include "../include/SyncNtpDelegate.h"

// If you want, you can define WiFi settings globally in Eclipse Environment Variables
#ifndef WIFI_SSID
	#define WIFI_SSID "PleaseEnterSSID" // Put you SSID and Password here
	#define WIFI_PWD "PleaseEnterPass"
#endif

#define INT_PIN 0   // GPIO0
#define PWM_PIN 2	// GPIO2
uint8_t pwm_pin[1] = { PWM_PIN }; // List of pins that you want to connect to pwm

HardwarePWM HW_pwm(pwm_pin, 1);
SyncNTP *syncNTP;
TelnetServer telnet;
SetPWMCmd setPWMCmd;



Timer procTimer;

//Geiger Counter Variables
uint32 event_counter;
uint32 actMeasureIntervall = 0;				// last measure intervall in us
uint32 setMeasureIntervall = 60000000;		// set value for measure intervall in us
bool doMeasure;

void IRAM_ATTR interruptHandler()
{
	event_counter++;
//	Debug.printf( "!!!Event\r\n");
}

void Loop() {
	uint32 actMicros = micros();
	auto actIntervall = actMicros - actMeasureIntervall;
	bool stopMeasure= false;

	if (setMeasureIntervall == 0) {
		if (event_counter >= 100) {
			stopMeasure = true;
		}
	} else {
		stopMeasure = true;
	}

	if (stopMeasure) {
		detachInterrupt(INT_PIN);
		actMeasureIntervall = actIntervall;
		// send Measurement
		Debug.printf("Events: %ld ",event_counter);
		Debug.printf("Interfall: %ld\r\n", actMeasureIntervall);
		sendData(event_counter, actMeasureIntervall);
		doMeasure = false;
		event_counter = 0;
		attachInterrupt(INT_PIN, interruptHandler, FALLING);
		actMeasureIntervall = actMicros;
		doMeasure = true;
		Debug.printf("start measure\r\n");
		if (setMeasureIntervall==0) {
			procTimer.setIntervalUs(100);
		} else {
			procTimer.setIntervalUs(setMeasureIntervall);
		}
	}
}

void setPWM(unsigned int duty) {
	if (duty <= 100) {
		auto ontime = duty*HW_pwm.getMaxDuty()/100;
		Debug.printf("pwm ontime: %d\r\n",ontime);
		HW_pwm.analogWrite(PWM_PIN, ontime);

	}
}
void setTime(unsigned int time) {
	if (time <= 3600) {
		uint32 timeus = time*1000000;
		Debug.printf("measuretime: %ld\r\n", timeus);
		setMeasureIntervall = timeus;
	}
}

void setTelnetDebugOn(String commandLine, CommandOutput* commandOutput)
{
	telnet.enableDebug(true);
	commandOutput->printf("debug telnet on\r\n");
	Debug.start();
	Debug.printf("This is debug after telnet start\r\n");

}
void setTelnetDebugOff(String commandLine, CommandOutput* commandOutput)
{
	telnet.enableDebug(false);
	commandOutput->printf("debug telnet off\r\n");
	Debug.stop();
	Debug.printf("This is debug after telnet start\r\n");
}

// Will be called when WiFi station was connected to AP
void connectOk()
{
	telnet.enableDebug(true);
	telnet.listen(23);

	commandHandler.registerCommand(CommandDelegate("debugtelneton","Set telnet debug on","Application",commandFunctionDelegate(&setTelnetDebugOn)));
	commandHandler.registerCommand(CommandDelegate("debugtelnetoff","Set telnet debug off","Application",commandFunctionDelegate(&setTelnetDebugOff)));

	debugf("\r\n=== TelnetServer SERVER STARTED ===");
	debugf("==============================\r\n");

	setPWMCmd.initCommand(SetPWMDelegate(&setPWM),SetTimeDelegate(&setTime));
	procTimer.start();
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
	Serial.systemDebugOutput(false); // Enable debug output to serial

	commandHandler.registerSystemCommands();
	spiffs_mount(); // Mount file system, in order to work with files

	// WIFI not needed for demo. So disabling WIFI.
	WifiStation.enable(true);
	WifiStation.config(WIFI_SSID, WIFI_PWD); // Put you SSID and Password here

	WifiAccessPoint.enable(false);

	// Setting PWM values on 8 different pins
	HW_pwm.setPeriod(400);
	HW_pwm.analogWrite(PWM_PIN, HW_pwm.getMaxDuty());

	procTimer.initializeMs(100,TimerDelegate(&Loop));


	// set timezone hourly difference to UTC
	SystemClock.setTimeZone(2);

	WifiStation.waitConnection(connectOk, 30, connectFail); // We recommend 20+ seconds at start
}
