/*
 * Debug.cpp
 *
 */

#include "../include/SetPWMCmd.h"

SetPWMCmd::SetPWMCmd()
{
	debugf("SetPWMCmd Instantiating");
}

SetPWMCmd::~SetPWMCmd()
{
}

void SetPWMCmd::initCommand(SetPWMDelegate pwmDelegate, SetTimeDelegate timeDelegate)
{
	commandHandler.registerCommand(CommandDelegate("setpwm","set pwm duty cycle","Application",commandFunctionDelegate(&SetPWMCmd::processSetPWMCmd,this)));
	commandHandler.registerCommand(CommandDelegate("settime","set measure time","Application",commandFunctionDelegate(&SetPWMCmd::processSetTime,this)));
	setPWM = pwmDelegate;
	setTime = timeDelegate;
}

void SetPWMCmd::processSetTime(String commandLine, CommandOutput* commandOutput)
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
			if (measureTime > 3600) measureTime = 3600;
			commandOutput->printf("measuretime is %d\r\n", measureTime);
		}
		if (setPWM) {
			setTime(measureTime);
		}
	}
}
void SetPWMCmd::processSetPWMCmd(String commandLine, CommandOutput* commandOutput)
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
			status = true;
			commandOutput->printf("pwm ON\r\n");
		} else if (commandToken[1] == "off") {
			status = false;
			commandOutput->printf("pwm OFF\r\n");
		} else if (commandToken[1] == "status") {
			String tempString = status ? "ON" : "OFF";
			commandOutput->printf("pwm is %s with %d%%\r\n",tempString.c_str(),duty);
		} else {
			auto value = commandToken[1];
			duty = value.toInt();
			if (duty > 100) duty = 100;
			commandOutput->printf("pwm is %d%%\r\n", duty);
		}
		if (setPWM) {
			if (status) {
				setPWM(duty);
			} else {
				setPWM(0);
			}
		}
	}
}


