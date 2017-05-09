#ifndef LOGGER_H
#define LOGGER_H

#include "stl_headers.h"
#include "printf.h"
#include "singleton.h"

class Logger : public Singleton<Logger> {
public:
	void setStream(std::ostream& os) {
		m_os = &os;
	}
	
	void setPrefix(const std::string& prefix) {
		m_prefix = prefix;
	}
	
	template <typename... Args>
	void log(const char* format, Args... args) const {
		printf(*m_os, (m_prefix + format + "\n").c_str(), args...);
	}


	Logger() : m_os(&std::cerr)
	{
	}
	
private:
	std::ostream* m_os;
	std::string m_prefix;
};

#endif