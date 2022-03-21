#include "Command.h"
#include <string>

using namespace std;

namespace Protocol {


	Command::Command(string command, string args[], int argsSize) {
		this->command = command;
		this->args = args;
		this->argsSize = argsSize;
	};

	string Command::getCommand() {
		return command;
	}

	string* Command::getArgs() {
		return args;
	}

	int Command::getArgsSize() {
		return argsSize;
	}

}