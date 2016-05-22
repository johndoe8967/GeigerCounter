/*
 * ExampleCommand.h
 *
 */

#ifndef SMINGCORE_EXAMPLE_COMMAND_H_
#define SMINGCORE_EXAMPLE_COMMAND_H_

#include "WString.h"
#include "../Services/CommandProcessing/CommandProcessingIncludes.h"
#include <SmingCore/Network/TelnetServer.h>


typedef Delegate<void(unsigned int pwmDuty)> SetPWMDelegate;
typedef Delegate<void(unsigned int pwmDuty)> SetTimeDelegate;

class CommandClass
{
public:
	CommandClass();
	virtual ~CommandClass();
	void init(SetPWMDelegate delegate, SetTimeDelegate delegate2);

private:
	TelnetServer *telnet;

	void SaveSettings();
	bool pwmState = false;
	unsigned int pwmDuty = 0;
	unsigned int measureTime = 60;
	void processSetPWMCmd(String commandLine, CommandOutput* commandOutput);
	void processSetTime(String commandLine, CommandOutput* commandOutput);
	void setTelnetDebugOn(String commandLine, CommandOutput* commandOutput);
	void setTelnetDebugOff(String commandLine, CommandOutput* commandOutput);

	SetPWMDelegate setPWM = null;
	SetTimeDelegate setTime = null;

};


#endif /* SMINGCORE_DEBUG_H_ */
