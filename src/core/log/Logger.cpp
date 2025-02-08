#include "Logger.hpp"

Logger& Logger::getInstance() {
	static Logger logger;
	return logger;
}

void Logger::enableFileLogging(bool enable) {
	log_to_file = enable;
}

Logger::Logger() {
	std::string filename = "logs/log_" + getTimeStamp() + ".log";
	enableFileLogging(true);
	file.open(filename, std::ios::out | std::ios::app);
	if (!file.is_open()) {
		std::cout << "LOGGER: Failed to open log file: " << filename << std::endl;
	}
}

Logger::~Logger() {
	if (file.is_open()) {
		file.close();
	}
}

void Logger::internalLog(Level level, const std::string& message, const std::string& file, int line) {
	std::lock_guard<std::mutex> lock(log_mutex);

	std::string log_entry = std::format("[{}] [{}] [{}:{}]: {}", getTimeStamp(), levelToString(level), file, line, message);
	std::cout << log_entry << std::endl;

	if (log_to_file && this->file.is_open()) {
		this->file << log_entry << std::endl;
	}
}

std::string Logger::getTimeStamp() {
	auto now = std::time(nullptr);
	char buffer[10];
	std::strftime(buffer, sizeof(buffer), "%I:%M %p", std::localtime(&now));
	return std::string(buffer);
}

std::string Logger::levelToString(Level level) {
	switch (level) {
		case Level::DEBUG: return "DEBUG";
		case Level::INFO: return "INFO";
		case Level::WARNING: return "WARNING";
		case Level::ERROR: return "ERROR";
		default: return "UNKNOWN";
	}
}