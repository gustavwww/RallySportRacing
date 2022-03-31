#include "Command.h"
#include <string>

using namespace std;

namespace Protocol {


	Command::Command(string command, vector<string> args) {
		this->command = command;
		this->args = args;
	};

	string Command::getCommand() {
		return command;
	}

	vector<string> Command::getArgs() {
		return args;
	}

	int Command::getArgsSize() {
		return args.size();
	}

}