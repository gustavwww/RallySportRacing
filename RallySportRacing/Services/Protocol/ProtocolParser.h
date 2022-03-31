#ifndef PROTOCOLPARSER
#define PROTOCOLPARSER

#include "Command.h"
#include <string>

namespace Protocol {

	Command parseMessage(string msg);
	string parseCommand(Command cmd);

}

#endif