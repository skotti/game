#ifndef RANDOM_H
#define RANDOM_H

#include "stl_headers.h"

class Random {
public:
	static int getInt(int a, int b) {
		auto dis = std::uniform_int_distribution<int>(a, b);
		return dis(m_rand);
	}
	
	static float getFloat(float a, float b) {
		auto dis = std::uniform_real_distribution<float>(a, b);
		return dis(m_rand);
	}
private:
	
	static std::mt19937_64 m_rand;
};

#endif