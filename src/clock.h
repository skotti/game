#ifndef CLOCK_H
#define CLOCK_H

#include "stl_headers.h"

class Clock {
public:
	Clock() { reset(); }
	
	void reset() {
		m_first = InternalClock::now();
	}
	
	double getTime() const {
		std::chrono::duration<double> elapsed = InternalClock::now() - m_first;
		return elapsed.count(); 
	}
	
private:
	typedef std::chrono::high_resolution_clock InternalClock;
	
	std::chrono::time_point<InternalClock> m_first;
};

#endif