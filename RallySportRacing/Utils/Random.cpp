//Writen by TheCherno
//Youtube: https://www.youtube.com/watch?v=GK0jHlv3e3w&list=WL&index=9
//Github: https://github.com/TheCherno/OneHourParticleSystem/blob/master/OpenGL-Sandbox/src/Random.cpp

#include "Random.h"

namespace Utils {
	std::mt19937 Random::s_RandomEngine;
	std::uniform_int_distribution<std::mt19937::result_type> Random::s_Distribution;
}

