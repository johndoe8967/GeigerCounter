/*
 * ExampleCommand.h
 *
 */

#ifndef SMINGCORE_EXAMPLE_COMMAND_H_
#define SMINGCORE_EXAMPLE_COMMAND_H_

#include "WString.h"
#include "../Services/CommandProcessing/CommandProcessingIncludes.h"

typedef Delegate<void(unsigned int duty)> SetPWMDelegate;

class SetPWMCmd
{
public:
	SetPWMCmd();
	virtual ~SetPWMCmd();
	void initCommand(SetPWMDelegate delegate);

private:
	bool status = true;
	unsigned int duty = 0;
	void processSetPWMCmd(String commandLine, CommandOutput* commandOutput);
	SetPWMDelegate setPWM;

};


#endif /* SMINGCORE_DEBUG_H_ */
