#ifndef UTILS_H
#define UTILS_H
#include <cmath>

#include <string>
#include <sstream>

#define EPS 1e-13


inline bool isEqual(float a, float b) {
	return (fabs(a - b) < EPS);
}


template <typename T>
T fromString(std::string param) 
{
	std::istringstream iss(param);
	T val = T();
	iss >> val;
	if (!iss.eof() || iss.fail()) throw std::runtime_error("Can't extract specified type from string");
	return val;
}

template <typename T>
std::string toString(T param)
{
	std::ostringstream oss;
	oss << param;
	return oss.str();
}

#endif
