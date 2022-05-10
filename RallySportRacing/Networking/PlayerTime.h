#ifndef PLAYERTIME
#define PLAYERTIME
#include <string>


using namespace std;

namespace Networking {


	class PlayerTime {
	public:
		PlayerTime(string name, float time);

		string getName();
		float getTime();

	private:
		string name;
		float time;

	};



}


#endif