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

void SetPWMCmd::initCommand(SetPWMDelegate delegate)
{
	commandHandler.registerCommand(CommandDelegate("setpwm","set pwm duty cycle","Application",commandFunctionDelegate(&SetPWMCmd::processSetPWMCmd,this)));
}

void SetPWMCmd::processSetPWMCmd(String commandLine, CommandOutput* commandOutput)
{
	Vector<String> commandToken;
	int numToken = splitString(commandLine, ' ' , commandToken);

	if (numToken == 1)
	{
		commandOutput->printf("set pwm commands available : \r\n");
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
			commandOutput->printf("pwm is %s\r\n",tempString.c_str());
		} else {
			auto value = commandToken[1];
			duty = value.toInt();
		}
		if (status) {
			setPWM(duty);
		} else {
			setPWM(0);
		}
	}
}


