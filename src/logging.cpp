#include <glpp/logging.hpp>

#include <iostream>

bool gl::Logging::LogToStderr = true;
std::unordered_set<gl::LoggingEndpoint*> gl::Logging::sEndpoints;

void gl::Logging::Dispatch(std::string file, int line, LogLevel_ level, std::string msg)
{
	gl::LogMessage message(file, line, level, msg);
	for (LoggingEndpoint* endpoint : sEndpoints) {
		endpoint->onMessage(message);
	}
	if (LogToStderr) {
		if (level == LogLevel_Success) {
			std::cerr << "\033[32m";
		}
		else if (level == LogLevel_Warning) {
			std::cerr << "\033[93m";
		}
		else if (level == LogLevel_Error) {
			std::cerr << "\033[31m";
		}
		std::cerr << "[" << message.file << "," << message.line << "]:\033[0m " << msg << std::endl;
	}
}

void gl::Logging::RegisterEndpoint(LoggingEndpoint* endpoint)
{
	if (sEndpoints.find(endpoint) != sEndpoints.end()) {
		Dispatch(__FILE__, __LINE__, LogLevel_Warning, "Tried to register already registered endpoint");
	}
	else {
		sEndpoints.insert(endpoint);
	}
}

void gl::Logging::RemoveEndpoint(LoggingEndpoint* endpoint)
{
	if (sEndpoints.find(endpoint) != sEndpoints.end()) {
		Dispatch(__FILE__, __LINE__, LogLevel_Warning, "Tried to remove endpoint but could not find it");
	}
	else {
		sEndpoints.erase(endpoint);
	}
}
