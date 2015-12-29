#pragma once
#include <fstream>

class Logger
{
public:
	enum class Priority {
		INFO,
		WARN,
		ERR,
		CRIT_ERR
	};
public:
	Logger(const char* file, Priority prio);
	~Logger();


	template<typename T>
	Logger& operator<<(const T& p) {
		if(outfile.good() && currPrio >= filter) { 
			outfile << p;
		}
		return *this;
	}

	template<>
	Logger& operator<<(const Priority& prio) {
		currPrio = prio;
		switch(prio) {
		case Priority::INFO:
			*this << "[INFO] ";
			break;
		case Priority::WARN:
			*this << "[WARNING] ";
			break;
		case Priority::ERR:
			*this << "[ERROR] ";
			break;
		case Priority::CRIT_ERR:
			*this << "[CRITICAL ERROR] ";
			break;
		}
		return *this;
	}

	void SetPriority(Priority prio);
private:
	std::ofstream outfile;
	Priority filter;
	Priority currPrio;
};

extern Logger g_Logger;