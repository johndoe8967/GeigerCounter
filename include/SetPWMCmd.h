/*
 * ExampleCommand.h
 *
 */

#ifndef SMINGCORE_EXAMPLE_COMMAND_H_
#define SMINGCORE_EXAMPLE_COMMAND_H_

#include "WString.h"
#include "../Services/CommandProcessing/CommandProcessingIncludes.h"

typedef Delegate<void(unsigned int duty)> SetPWMDelegate;
typedef Delegate<void(unsigned int duty)> SetTimeDelegate;

class SetPWMCmd
{
public:
	SetPWMCmd();
	virtual ~SetPWMCmd();
	void initCommand(SetPWMDelegate delegate, SetTimeDelegate delegate2);

private:
	bool status = true;
	unsigned int duty = 0;
	uint32 measureTime = 60;
	void processSetPWMCmd(String commandLine, CommandOutput* commandOutput);
	void processSetTime(String commandLine, CommandOutput* commandOutput);
	SetPWMDelegate setPWM = null;
	SetTimeDelegate setTime = null;

};


#endif /* SMINGCORE_DEBUG_H_ */
