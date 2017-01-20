#ifndef LOGGING_H
#define LOGGING_H

//	keeps the implementation of a program log file in one place

#include <string>
#include <fstream>

enum class logging {
	error,				//	only output error messages
	standard,			//	errors and standard program messages
	debug,				//	adds debug information
	full,				//	now with internal state data used during development
	extra				//	internal state of no longer current interest, but might be needed later
};

extern std::ofstream logfile;
extern logging loglevel;
extern bool logging_suppress;

void open_log(const std::string & filename, const logging level, const std::string & starttext);
std::string logsuppress(const std::string & s);

#endif