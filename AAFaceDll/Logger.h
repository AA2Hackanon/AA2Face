#pragma once
#include <fstream>
#include <Windows.h>

#define LOGPRIONC(prio) if(g_Logger.FilterPriority(prio)) g_Logger << prio << 
#define LOGPRIOC(prio) if(g_Logger.FilterPriority(prio)) g_Logger << prio << __FUNCSIC __ ": " <<
#define LOGPRIO(prio) if(g_Logger.FilterPriority(prio)) g_Logger << prio << \
	(g_Logger.FilterPriority(Logger::Priority::SPAM) ? ("[" __FUNCSIG__ "]:\r\n\t") : "")

class Logger
{
public:
	enum class Priority {
		SPAM,
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
		case Priority::SPAM:
			*this << "[SPAM] ";
			break;
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

	Logger& operator<<(const BYTE b) {
		*this << (int)b;
		return *this;
	}

	void SetPriority(Priority prio);
	bool FilterPriority(Priority prio);
private:
	std::ofstream outfile;
	Priority filter;
	Priority currPrio;
};

extern Logger g_Logger;