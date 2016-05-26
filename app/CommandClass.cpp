/*
 * Debug.cpp
 *
 */

#include "../include/CommandClass.h"
#include "../include/AppSettings.h"

CommandClass::CommandClass()
{
	debugf("SetPWMCmd Instantiating");
}

CommandClass::~CommandClass()
{
}

void CommandClass::SaveSettings() {
	AppSettings.measureTime = measureTime;
	AppSettings.pwmDuty = pwmDuty;
	AppSettings.pwmState = pwmState;
	AppSettings.save();
}

void CommandClass::init(SetPWMDelegate pwmDelegate, SetTimeDelegate timeDelegate, SetDoseRatioDelegate doseDelegate)
{
	telnet = new TelnetServer();
	telnet->enableDebug(true);
	telnet-> listen(23);
	debugf("\r\n=== TelnetServer SERVER STARTED ===");
	debugf("==============================\r\n");


	AppSettings.load();

	if (AppSettings.exist()) {
		measureTime = AppSettings.measureTime;
		pwmDuty = AppSettings.pwmDuty;
		pwmState = AppSettings.pwmState;
		doseRatio = AppSettings.doseRatio;
	} else {
		SaveSettings();
	}

	commandHandler.registerCommand(CommandDelegate("setpwm","set pwm duty cycle","Application",commandFunctionDelegate(&CommandClass::processSetPWMCmd,this)));
	commandHandler.registerCommand(CommandDelegate("settime","set measure time","Application",commandFunctionDelegate(&CommandClass::processSetTime,this)));
	commandHandler.registerCommand(CommandDelegate("setdoseratio","set cpm/uSv ratio","Application",commandFunctionDelegate(&CommandClass::processSetDoseRatio,this)));
	commandHandler.registerCommand(CommandDelegate("settsapi","set thingspeak API","Application",commandFunctionDelegate(&CommandClass::processSetTSAPI,this)));
	commandHandler.registerCommand(CommandDelegate("debugtelneton","Set telnet debug on","Application",commandFunctionDelegate(&CommandClass::setTelnetDebugOn,this)));
	commandHandler.registerCommand(CommandDelegate("debugtelnetoff","Set telnet debug off","Application",commandFunctionDelegate(&CommandClass::setTelnetDebugOff,this)));

	setPWM = pwmDelegate;
	if (setPWM) {
		if (pwmState) {
			setPWM(pwmDuty);
		} else {
			setPWM(0);
		}
	}

	setTime = timeDelegate;
	if (setTime) {
		setTime(measureTime);
	}

	setDoseRatio = doseDelegate;
	if (setDoseRatio) {
		setDoseRatio(doseRatio);
	}

}

void CommandClass::setTelnetDebugOn(String commandLine, CommandOutput* commandOutput)
{
	telnet->enableDebug(true);
	commandOutput->printf("debug telnet on\r\n");
	Debug.start();
	Debug.printf("This is debug after telnet start\r\n");

}
void CommandClass::setTelnetDebugOff(String commandLine, CommandOutput* commandOutput)
{
	telnet->enableDebug(false);
	commandOutput->printf("debug telnet off\r\n");
	Debug.stop();
	Debug.printf("This is debug after telnet start\r\n");
}


void CommandClass::processSetTime(String commandLine, CommandOutput* commandOutput)
{
	Vector<String> commandToken;
	int numToken = splitString(commandLine, ' ' , commandToken);

	if (numToken == 1)
	{
		commandOutput->printf("settime commands available : \r\n");
		commandOutput->printf("auto : auto measure until 100 events\r\n");
		commandOutput->printf("status : Show pwm status\r\n");
		commandOutput->printf("<value>: Set time in seconds\r\n");
	}
	else
	{
		if (commandToken[1] == "auto") {
			measureTime = 0;
			commandOutput->printf("measuretime auto\r\n");
		} else if (commandToken[1] == "status") {
			String tempString = measureTime==0 ? "AUTO" : String(measureTime);
			commandOutput->printf("measuretime is %s\r\n",tempString.c_str());
		} else {
			auto value = commandToken[1];
			measureTime = value.toInt();
			if (measureTime > 600) measureTime = 600;
			commandOutput->printf("measuretime is %d\r\n", measureTime);
		}
		if (setTime) {
			setTime(measureTime);
		}
		SaveSettings();
	}
}
void CommandClass::processSetPWMCmd(String commandLine, CommandOutput* commandOutput)
{
	Vector<String> commandToken;
	int numToken = splitString(commandLine, ' ' , commandToken);

	if (numToken == 1)
	{
		commandOutput->printf("setpwm commands available : \r\n");
		commandOutput->printf("on   : Set pwm ON\r\n");
		commandOutput->printf("off  : Set pwm OFF\r\n");
		commandOutput->printf("status : Show pwm status\r\n");
		commandOutput->printf("<value>: Set duty cycle\r\n");
	}
	else
	{
		if (commandToken[1] == "on") {
			pwmState = true;
			commandOutput->printf("pwm ON\r\n");
		} else if (commandToken[1] == "off") {
			pwmState = false;
			commandOutput->printf("pwm OFF\r\n");
		} else if (commandToken[1] == "status") {
			String tempString = pwmState ? "ON" : "OFF";
			commandOutput->printf("pwm is %s with %d%%\r\n",tempString.c_str(),pwmDuty);
		} else {
			auto value = commandToken[1];
			pwmDuty = value.toInt();
			if (pwmDuty > 100) pwmDuty = 100;
			commandOutput->printf("pwm is %d%%\r\n", pwmDuty);
		}
		if (setPWM) {
			if (pwmState) {
				setPWM(pwmDuty);
			} else {
				setPWM(0);
			}
		}
		SaveSettings();
	}
}


void CommandClass::processSetDoseRatio(String commandLine, CommandOutput* commandOutput)
{
	Vector<String> commandToken;
	int numToken = splitString(commandLine, ' ' , commandToken);

	if (numToken == 1)
	{
		commandOutput->printf("setdoseratio commands available : \r\n");
		commandOutput->printf("status : Show cpm/uSv value\r\n");
		commandOutput->printf("<value>: Set cpm/uSv value\r\n");
	}
	else
	{
		if (commandToken[1] == "status") {
			commandOutput->printf("cpm/uSv %f\r\n",doseRatio);
		} else {
			auto value = commandToken[1];
			doseRatio = value.toFloat();
			commandOutput->printf("cpm/uSv %f\r\n",doseRatio);
		}
		if (setDoseRatio) {
			setDoseRatio(doseRatio);
		}
		SaveSettings();
	}
}

void CommandClass::processSetTSAPI(String commandLine, CommandOutput* commandOutput)
{
	Vector<String> commandToken;
	int numToken = splitString(commandLine, ' ' , commandToken);

	if (numToken == 1)
	{
		commandOutput->printf("settsapi commands available : \r\n");
		commandOutput->printf("status : Show ThingSpeak API\r\n");
		commandOutput->printf("<value>: Set ThingSpeak API\r\n");
	}
	else
	{
		if (commandToken[1] == "status") {
			commandOutput->printf("API %s\r\n",AppSettings.tsAPI);
		} else {
			auto value = commandToken[1];
			AppSettings.tsAPI = value;
			commandOutput->printf("API %s\r\n",AppSettings.tsAPI.c_str());
		}
		SaveSettings();

	}
}

