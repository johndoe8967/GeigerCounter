/*
 * CommandClass.cpp
 * Original Author: https://github.com/johndoe8967
 *
 * settime | Application | set measure time
 * setdoseratio | Application | set cpm/uSv ratio
 * settsapi | Application | set thingspeak API
 * setssid | Application | set wifi ssid
 * debugtelneton | Application | Set telnet debug on
 * debugtelnetoff | Application | Set telnet debug off
 *
 *
 * Created on June 5, 2016
 */

#include "../include/CommandClass.h"
#include "../include/AppSettings.h"

CommandClass::CommandClass()
{
	debugf("CommandClass Instantiating");
	telnet = new TelnetServer();

}

CommandClass::~CommandClass()
{
	delete(telnet);
}

void CommandClass::SaveSettings() {
	AppSettings.measureTime = measureTime;
	AppSettings.doseRatio = doseRatio;
	AppSettings.save();
}

void CommandClass::init(SetTimeDelegate timeDelegate)
{
	telnet->enableDebug(true);
	telnet-> listen(23);
	debugf("\r\n=== TelnetServer SERVER STARTED ===");
	debugf("==============================\r\n");

	if (AppSettings.exist()) {
		measureTime = AppSettings.measureTime;
		doseRatio = AppSettings.doseRatio;
	} else {
		AppSettings.tsAPI = "---";
		SaveSettings();
	}

	commandHandler.registerCommand(CommandDelegate("settime","set measure time","Application",commandFunctionDelegate(&CommandClass::processSetTime,this)));
	commandHandler.registerCommand(CommandDelegate("setdoseratio","set cpm/uSv ratio","Application",commandFunctionDelegate(&CommandClass::processSetDoseRatio,this)));
	commandHandler.registerCommand(CommandDelegate("settsapi","set thingspeak API","Application",commandFunctionDelegate(&CommandClass::processSetTSAPI,this)));
	commandHandler.registerCommand(CommandDelegate("settsapiDust","set thingspeak API Dust","Application",commandFunctionDelegate(&CommandClass::processSetTSAPIDust,this)));
	commandHandler.registerCommand(CommandDelegate("setssid","set wifi ssid","Application",commandFunctionDelegate(&CommandClass::processSetWIFISSID,this)));
	commandHandler.registerCommand(CommandDelegate("setpwd","set wifi pwd","Application",commandFunctionDelegate(&CommandClass::processSetWIFIPWD,this)));
	commandHandler.registerCommand(CommandDelegate("debugtelneton","Set telnet debug on","Application",commandFunctionDelegate(&CommandClass::setTelnetDebugOn,this)));
	commandHandler.registerCommand(CommandDelegate("debugtelnetoff","Set telnet debug off","Application",commandFunctionDelegate(&CommandClass::setTelnetDebugOff,this)));

	setTime = timeDelegate;
	if (setTime) {
		setTime(measureTime);
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
		commandOutput->printf("auto : auto measure until 100 events\r\n");
		commandOutput->printf("status : Show time status\r\n");
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

void CommandClass::processSetDoseRatio(String commandLine, CommandOutput* commandOutput)
{
	Vector<String> commandToken;
	int numToken = splitString(commandLine, ' ' , commandToken);

	if (numToken == 1)
	{
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
		SaveSettings();
	}
}
void CommandClass::processTSAPI(String commandLine, CommandOutput* commandOutput, String *tsAPI) {
	Vector<String> commandToken;
	int numToken = splitString(commandLine, ' ' , commandToken);

	if (numToken == 1)
	{
		commandOutput->printf("status : Show ThingSpeak API\r\n");
		commandOutput->printf("<value>: Set ThingSpeak API\r\n");
	}
	else
	{
		if (commandToken[1] == "status") {
			commandOutput->printf("API %s\r\n",tsAPI->c_str());
		} else {
			auto value = commandToken[1];
			*tsAPI = value;
			commandOutput->printf("API %s\r\n",tsAPI->c_str());
		}
		SaveSettings();
	}
}
void CommandClass::processSetTSAPIDust(String commandLine, CommandOutput* commandOutput)
{
	processTSAPI(commandLine, commandOutput, &AppSettings.tsAPIDust);
}

void CommandClass::processSetTSAPI(String commandLine, CommandOutput* commandOutput)
{
	processTSAPI(commandLine, commandOutput, &AppSettings.tsAPI);
}

void CommandClass::processSetWIFISSID(String commandLine, CommandOutput* commandOutput)
{
	Vector<String> commandToken;
	int numToken = splitString(commandLine, ' ' , commandToken);

	if (numToken == 2)
	{
		auto value = commandToken[1];
		AppSettings.WLANSSID = value;
		commandOutput->printf("SSID %s\r\n",AppSettings.WLANSSID.c_str());
		SaveSettings();
	}
}
void CommandClass::processSetWIFIPWD(String commandLine, CommandOutput* commandOutput)
{
	Vector<String> commandToken;
	int numToken = splitString(commandLine, ' ' , commandToken);

	if (numToken == 2)
	{
		auto value = commandToken[1];
		AppSettings.WLANPWD = value;
		commandOutput->printf("PWD %s\r\n",AppSettings.WLANPWD.c_str());
		SaveSettings();
	}
}
