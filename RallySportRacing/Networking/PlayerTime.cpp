#include "PlayerTime.h"


using namespace std;

namespace Networking {

	PlayerTime::PlayerTime(string name, float time) {
		this->name = name;
		this->time = time;
	}

	string PlayerTime::getName() {
		return name;
	}

	float PlayerTime::getTime() {
		return time;
	}

}