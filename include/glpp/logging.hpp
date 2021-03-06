#pragma once

#include <iostream>
#include <stdexcept>
#include <string>
#include <unordered_set>
#include <unordered_map>
#include <vector>

#define LOG(msg) gl::Logging::Dispatch(__FILE__, __LINE__, gl::LogLevel_Info, (msg))
#define LOG(format, ...) gl::Logging::Dispatchf(__FILE__, __LINE__, gl::LogLevel_Info, format, __VA_ARGS__)
#define LOG_IF(cond, msg) if((cond)) { LOG(msg); }
#define LOG_IF(cond, format, ...) if((cond)) { LOG(format, __VA_ARGS__); }
#define LOG_ONCE(msg) { static bool __first_call__ = true; LOG_IF(__first_call__, msg); __first_call__ = false; }
#define LOG_ONCE(format, ...) { static bool __first_call__ = true; LOG_IF(__first_call__, format, __VA_ARGS__); __first_call__ = false; }
#define LOG_EVERY_N(n, msg) { static ptrdiff_t __callcount__ = 0; LOG_IF(__callcount__ % (n) == 0, msg);  __callcount__++; }
#define LOG_EVERY_N(n, format, ...) { static ptrdiff_t __callcount__ = 0; LOG_IF(__callcount__ % (n) == 0, format, __VA_ARGS__);  __callcount__++; }
#if !defined(_DEBUG)
#define DEBUG_LOG(msg)
#define DEBUG_LOG(msg, ...)
#define DEBUG_LOG_IF(cond, msg)
#else
#define DEBUG_LOG(msg) LOG((msg))
#define DEBUG_LOG(msg, ...) LOG((msg), __VA_ARGS__)
#define DEBUG_LOG_IF(cond, msg) LOG_IF(cond, msg)
#endif

#define LOG_SUCCESS(msg) gl::Logging::Dispatch(__FILE__, __LINE__, gl::LogLevel_Success, (msg))
#define LOG_SUCCESS(format, ...) gl::Logging::Dispatchf(__FILE__, __LINE__, gl::LogLevel_Success, format, __VA_ARGS__)
#define LOG_SUCCESS_IF(msg) if((cond)) { LOG_SUCCESS(msg); }
#define LOG_SUCCESS_IF(cond, format, ...) if((cond)) { LOG_SUCCESS(format, __VA_ARGS__); }
#define LOG_SUCCESS_ONCE(msg) { static bool __first_call__ = true; LOG_SUCCESS_IF(__first_call__, msg); __first_call__ = false; }
#define LOG_SUCCESS_ONCE(format, ...) { static bool __first_call__ = true; LOG_SUCCESS_IF(__first_call__, format, __VA_ARGS__); __first_call__ = false; }
#define LOG_SUCCESS_EVERY_N(n, msg) { static ptrdiff_t __callcount__ = 0; LOG_SUCCESS_IF(__callcount__ % (n) == 0, msg);  __callcount__++; }
#define LOG_SUCCESS_EVERY_N(n, format, ...) { static ptrdiff_t __callcount__ = 0; LOG_SUCCESS_IF(__callcount__ % (n) == 0, format, __VA_ARGS__);  __callcount__++; }

#define LOG_WARNING(msg) gl::Logging::Dispatch(__FILE__, __LINE__, gl::LogLevel_Warning, (msg))
#define LOG_WARNING(format, ...) gl::Logging::Dispatchf(__FILE__, __LINE__, gl::LogLevel_Warning, format, __VA_ARGS__)
#define LOG_WARNING_IF(msg) if((cond)) { LOG_WARNING(msg); }
#define LOG_WARNING_IF(cond, format, ...) if((cond)) { LOG_WARNING(format, __VA_ARGS__); }
#define LOG_WARNING_ONCE(msg) { static bool __first_call__ = true; LOG_WARNING_IF(__first_call__, msg); __first_call__ = false; }
#define LOG_WARNING_ONCE(format, ...) { static bool __first_call__ = true; LOG_WARNING_IF(__first_call__, format, __VA_ARGS__); __first_call__ = false; }
#define LOG_WARNING_EVERY_N(n, msg) { static ptrdiff_t __callcount__ = 0; LOG_WARNING_IF(__callcount__ % (n) == 0, msg);  __callcount__++; }
#define LOG_WARNING_EVERY_N(n, format, ...) { static ptrdiff_t __callcount__ = 0; LOG_WARNING_IF(__callcount__ % (n) == 0, format, __VA_ARGS__);  __callcount__++; }

#define LOG_ERROR(msg) gl::Logging::Dispatch(__FILE__, __LINE__, gl::LogLevel_Error, (msg))
#define LOG_EROR(format, ...) gl::Logging::Dispatchf(__FILE__, __LINE__, gl::LogLevel_Error, format, __VA_ARGS__)
#define LOG_ERROR_IF(msg) if((cond)) { LOG_ERROR(msg); }
#define LOG_ERROR_IF(cond, format, ...) if((cond)) { LOG_ERROR(format, __VA_ARGS__); }
#define LOG_ERROR_ONCE(msg) { static bool __first_call__ = true; LOG_ERROR_IF(__first_call__, msg); __first_call__ = false; }
#define LOG_ERROR_ONCE(format, ...) { static bool __first_call__ = true; LOG_ERROR_IF(__first_call__, format, __VA_ARGS__); __first_call__ = false; }
#define LOG_ERROR_EVERY_N(n, msg) { static ptrdiff_t __callcount__ = 0; LOG_ERROR_IF(__callcount__ % (n) == 0, msg);  __callcount__++; }
#define LOG_ERROR_EVERY_N(n, format, ...) { static ptrdiff_t __callcount__ = 0; LOG_ERROR_IF(__callcount__ % (n) == 0, format, __VA_ARGS__);  __callcount__++; }

#define LOG_SUCCESS_OR_WARNING(cond, msg_success, msg_fail) if((cond)) { LOG_SUCCESS(msg_success); } else { LOG_WARNING(msg_fail); }
#define LOG_SUCCESS_OR_ERROR(cond, msg_success, msg_fail) if((cond)) { LOG_SUCCESS(msg_success); } else { LOG_ERROR(msg_fail); }

// Checks
#if !defined(CHECKS_IN_RELEASE) && !defined(_DEBUG)
#define CHECK(cond, msg)
#else
#define CHECK(cond, msg) if(!(cond)) { \
	std::string text = std::string("[") + __FILE__ + "," + std::to_string(__LINE__) + "]: " + (msg); \
	std::cerr << "\033[31m" << text << "\033[0m" <<std::endl; \
	throw std::runtime_error(text); }
#endif

namespace gl {

	typedef int LogLevel;

	enum LogLevel_ {
		LogLevel_Info      = 0,
		LogLevel_Success   = 1,
		LogLevel_Warning   = 2,
		LogLevel_Error     = 3
	};

	struct LogMessage {
		std::string file;
		int line;
		LogLevel_ level;
		std::string msg;

	private:
		friend class Logging;
		LogMessage(std::string file, int line, LogLevel_ level, std::string msg) :
			file(file),
			line(line),
			level(level),
			msg(msg) {}
	};
	

	struct LoggingEndpoint {
		virtual void onMessage(const LogMessage& msg) = 0;
	};


	struct Logging {
		static void Dispatch(std::string file, int line, LogLevel_ level, std::string msg);
		template<typename... Args>
		static void Dispatchf(std::string file, int line, LogLevel_ level, const char* format, Args... args) {
			char tmp_buffer[3 * 1024] = { 0 };
			std::snprintf(tmp_buffer, sizeof(tmp_buffer), format, args...);
			Dispatch(file, line, level, std::string(tmp_buffer));
		}
		
		static void RegisterEndpoint(LoggingEndpoint* endpoint);
		static void RemoveEndpoint(LoggingEndpoint* endpoint);

		static bool LogToStderr;
	private:
		static std::unordered_set<LoggingEndpoint*> sEndpoints;
	};

}