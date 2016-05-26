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
typedef Delegate<void(unsigned int time)> SetTimeDelegate;
typedef Delegate<void(float newDoseRatio)> SetDoseRatioDelegate;


class CommandClass
{
public:
	CommandClass();
	virtual ~CommandClass();
	void init(SetPWMDelegate delegate, SetTimeDelegate delegate2, SetDoseRatioDelegate delegate3);

private:
	TelnetServer *telnet;

	void SaveSettings();
	bool pwmState = false;
	unsigned int pwmDuty = 0;
	unsigned int measureTime = 60;
	float doseRatio = 8000;				// 100 Impulse / s ==> 0,0045R/h ==> 45uSv/h
										// 6000 Imuplse / min ==> 0,0045R/h ==> 45uSv/h
										// 360000 Impulse / h ==> 0,0045R/h ==> 45uSv/h
										// => 8000 Impulse / uSv

	void processSetPWMCmd(String commandLine, CommandOutput* commandOutput);
	void processSetTime(String commandLine, CommandOutput* commandOutput);
	void processSetDoseRatio(String commandLine, CommandOutput* commandOutput);
	void processSetTSAPI(String commandLine, CommandOutput* commandOutput);
	void setTelnetDebugOn(String commandLine, CommandOutput* commandOutput);
	void setTelnetDebugOff(String commandLine, CommandOutput* commandOutput);

	SetPWMDelegate setPWM = null;
	SetTimeDelegate setTime = null;
	SetDoseRatioDelegate setDoseRatio = null;

};


#endif /* SMINGCORE_DEBUG_H_ */
