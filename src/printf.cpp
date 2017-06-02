#include "printf.h"

size_t printf(std::ostream& stream, const char* format) {
	stream << format;
}
