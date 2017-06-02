#ifndef PRINTF_H
#define PRINTF_H

#include "stl_headers.h"

template<typename T>
void printVal(std::ostream& stream, T val) {
	stream << val;
}

size_t printf(std::ostream& stream, const char* format);

template <typename T, typename... Args>
void printf(std::ostream& stream, const char* format, T val, Args... args) {
	while (*format) {
		if (*format != '%') {
			stream.put(*format++);
		} else {
			if (*(format+1) == '%') {
				stream.put(*format);
				format += 2;
			} else {
				printVal(stream, val);
				printf(stream, format+1, args...);
				break;
			}
		}
	}
}

template <typename... Args>
void printf(const char* format, Args... args) {
	printf(std::cout, format, args...);
}

#endif