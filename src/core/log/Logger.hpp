#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <chrono>
#include <mutex>
#include <format>
#include <ctime>

class Logger {
public:
	enum class Level : int {
		DEBUG,
		INFO,
		WARNING,
		ERROR
	};
	
	template<typename... Args>
	static void log(Level level, const std::string& file, int line, Args&&... args);

	static Logger& getInstance();

	void enableFileLogging(bool enable);

private:
	Logger();
	~Logger();
	Logger(const Logger& other) = delete;
	Logger& operator=(const Logger& other) = delete;

	void internalLog(Level level, const std::string& message, const std::string& file, int line);
	std::string getTimeStamp();
	std::string levelToString(Level level);

	std::ofstream file;
	std::mutex log_mutex;
	bool log_to_file;
};

template<typename... Args>
inline void Logger::log(Level level, const std::string& file, int line, Args&&... args) {
	std::ostringstream oss;
	(oss << ... << std::forward<Args>(args));
	getInstance().internalLog(level, oss.str(), file, line);
}

#define LOG_DEBUG(...) Logger::log(Logger::Level::DEBUG, __FILE__, __LINE__, __VA_ARGS__);
#define LOG_INFO(...) Logger::log(Logger::Level::INFO, __FILE__, __LINE__, __VA_ARGS__);
#define LOG_WARNING(...) Logger::log(Logger::Level::WARNING, __FILE__, __LINE__, __VA_ARGS__);
#define LOG_ERROR(...) Logger::log(Logger::Level::ERROR, __FILE__, __LINE__, __VA_ARGS__);

