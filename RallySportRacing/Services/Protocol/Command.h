#ifndef COMMAND
#define COMMAND

#include <string>
#include <vector>

using namespace std;

namespace Protocol {

	class Command {
	public:
		Command(string command, vector<string> args);
		string getCommand();
		vector<string> getArgs();
		int getArgsSize();
	private:
		string command;
		vector<string> args;
	};

}


#endif


