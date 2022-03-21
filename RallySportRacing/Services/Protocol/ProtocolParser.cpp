#include "ProtocolParser.h"
#include <vector>

using namespace std;

namespace Protocol {

	// Not currently used
	void trim(string &str) {
		str.erase(str.begin(), find_if(str.begin(), str.end(), [](unsigned char c) {
			return !isspace(c);
			}));

		str.erase(find_if(str.begin(), str.end(), [](unsigned char c) {
			return !isspace(c);
			}), str.end());

	}
	// ------------

	Command parseMessage(string msg) {

		string command = "";
		vector<string> args;

		string temp = "";
		for (unsigned char c : msg) {
			if (isspace(c)) {
				continue;
			}

			if (c == ':' && command == "") {
				command = temp;
				temp = "";
				continue;
			}

			if (c == ',') {
				args.push_back(temp);
				temp = "";
				continue;
			}

			temp += c;
		}

		if (command == "") {
			command = temp;
		} else {
			args.push_back(temp);
		}

		return Command(command, args);
	}

	string parseCommand(Command cmd) {
		string msg = cmd.getCommand() + ":";
		vector<string> args = cmd.getArgs();
		for (int i = 0; i < cmd.getArgsSize(); i++) {
			msg += args[i] + ",";
		}
		msg.erase(msg.end()-1);
		return msg;
	}

}