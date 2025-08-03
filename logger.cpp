// logger.cpp

#include "logger.h"
#include <iomanip>
#include <sstream>
#include <ctime>

Logger::Logger(const std::string& filename) 
    : log_filename(filename), is_logging_enabled(true) {
    openLogFile(filename);
}

Logger::~Logger() {
    closeLogFile();
}

bool Logger::openLogFile(const std::string& filename) {
    closeLogFile(); // close existing file
    
    log_filename = filename;
    log_file.open(filename, std::ios::out | std::ios::app);
    
    if (log_file.is_open()) {
        log("=== Tank War Game Log Started ===");
        return true;
    }
    
    std::cerr << "Warning: Cannot open log file: " << filename << std::endl;
    return false;
}

void Logger::closeLogFile() {
    if (log_file.is_open()) {
        log("=== Tank War Game Log Ended ===");
        log_file.close();
    }
}

void Logger::enableLogging(bool enable) {
    is_logging_enabled = enable;
}

void Logger::log(const std::string& message) {
    if (!is_logging_enabled) return;
    
    std::string timestamp = getCurrentTimestamp();
    
    // input
    if (log_file.is_open()) {
        log_file << "[" << timestamp << "] " << message << std::endl;
    }
    
    // also output to the terminal
    // std::cout << "[LOG] " << message << std::endl;
}

void Logger::logTurn(int turn_number) {
    std::stringstream ss;
    ss << "Turn " << turn_number << " started";
    log(ss.str());
}

void Logger::logTankMove(char tank_id, int x, int y, const std::string& direction) {
    std::stringstream ss;
    ss << "Tank " << tank_id << " moved to (" << x << ", " << y 
       << ") facing " << direction;
    log(ss.str());
}

void Logger::logTankShoot(char tank_id, int bullet_x, int bullet_y) {
    std::stringstream ss;
    ss << "Tank " << tank_id << " shot bullet at (" << bullet_x << ", " << bullet_y << ")";
    log(ss.str());
}

void Logger::logBulletMove(int x, int y, const std::string& direction) {
    std::stringstream ss;
    ss << "Bullet moved to (" << x << ", " << y << ") direction " << direction;
    log(ss.str());
}

void Logger::logBulletHit(char tank_id, int damage) {
    std::stringstream ss;
    ss << "Tank " << tank_id << " hit by bullet, took " << damage << " damage";
    log(ss.str());
}

void Logger::logTankDamage(char tank_id, int remaining_life, const std::string& reason) {
    std::stringstream ss;
    ss << "Tank " << tank_id << " damaged (" << reason 
       << "), remaining life: " << remaining_life;
    log(ss.str());
}

void Logger::logMapShrink(int new_size) {
    std::stringstream ss;
    ss << "Map shrunk to size " << new_size << "x" << new_size;
    log(ss.str());
}

void Logger::logGameResult(const std::string& result) {
    std::stringstream ss;
    ss << "Game ended: " << result;
    log(ss.str());
}

void Logger::logGameStart(const std::string& mode, int initial_life) {
    std::stringstream ss;
    ss << "Game started - Mode: " << mode << ", Initial Life: " << initial_life;
    log(ss.str());
}

void Logger::logError(const std::string& error_message) {
    std::stringstream ss;
    ss << "ERROR: " << error_message;
    log(ss.str());
}

std::string Logger::getCurrentTimestamp() const {
    auto now = std::time(nullptr);
    auto tm = *std::localtime(&now);
    
    std::stringstream ss;
    ss << std::put_time(&tm, "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

void Logger::flush() {
    if (log_file.is_open()) {
        log_file.flush();
    }
}