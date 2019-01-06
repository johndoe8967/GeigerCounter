/*
 * CommandClass.h
 *
 */

#ifndef SMINGCORE_EXAMPLE_COMMAND_H_
#define SMINGCORE_EXAMPLE_COMMAND_H_

#include "WString.h"
#include "../Services/CommandProcessing/CommandProcessingIncludes.h"
#include <SmingCore/Network/TelnetServer.h>


typedef Delegate<void(unsigned int time)> SetTimeDelegate;


class CommandClass
{
public:
	CommandClass();
	virtual ~CommandClass();
	void init(SetTimeDelegate delegate2);

private:
	TelnetServer *telnet;

	void SaveSettings();
	unsigned int measureTime = 60;
	float doseRatio = 120.0;			// 100 Impulse / s ==> 0,005R/h ==> 50uSv/h
										// 6000 Imuplse / min ==> 0,0050R/h ==> 50uSv/h
										// 360000 Impulse / h ==> 0,0050R/h ==> 50uSv/h
										// => 120 Impulse/min / uSv/h

	void processSetTime(String commandLine, CommandOutput* commandOutput);
	void processSetDoseRatio(String commandLine, CommandOutput* commandOutput);
	void processSetTSAPI(String commandLine, CommandOutput* commandOutput);
	void processSetTSAPIDust(String commandLine, CommandOutput* commandOutput);
	void processTSAPI(String commandLine, CommandOutput* commandOutput, String *tsAPI);
	void processSetWIFIPWD(String commandLine, CommandOutput* commandOutput);
	void processSetWIFISSID(String commandLine, CommandOutput* commandOutput);
	void setTelnetDebugOn(String commandLine, CommandOutput* commandOutput);
	void setTelnetDebugOff(String commandLine, CommandOutput* commandOutput);

	SetTimeDelegate setTime = null;

};


#endif /* SMINGCORE_DEBUG_H_ */
