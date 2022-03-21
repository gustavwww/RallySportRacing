#ifndef COMMAND
#define COMMAND

#include <string>

using namespace std;

namespace Protocol {

	class Command {
	public:
		Command(string command, string args[], int argsSize);
		string getCommand();
		string* getArgs();
		int getArgsSize();
	private:
		string command;
		string* args;
		int argsSize;
	};

}


#endif


