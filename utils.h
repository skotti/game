#ifndef UTILS_H
#define UTILS_H
#include <cmath>

#define EPS 1e-13

inline bool isEqual(float a, float b) {
	return (fabs(a - b) < EPS);
}

#endif