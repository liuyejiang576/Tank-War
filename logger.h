// logger.h

#ifndef LOGGER_H
#define LOGGER_H

#include <string>
#include <fstream>
#include <iostream>

class Logger {
private:
    std::string log_filename;
    std::ofstream log_file;
    bool is_logging_enabled;

public:
    Logger(const std::string& filename = "tankwar.log");
    ~Logger();
    
    bool openLogFile(const std::string& filename);
    void closeLogFile();
    void enableLogging(bool enable);
    
    void log(const std::string& message);
    void logTurn(int turn_number);
    void logTankMove(char tank_id, int x, int y, const std::string& direction);
    void logTankShoot(char tank_id, int bullet_x, int bullet_y);
    void logBulletMove(int x, int y, const std::string& direction);
    void logBulletHit(char tank_id, int damage);
    void logTankDamage(char tank_id, int remaining_life, const std::string& reason);
    void logMapShrink(int new_size);
    void logGameResult(const std::string& result);
    void logGameStart(const std::string& mode, int initial_life);
    void logError(const std::string& error_message);
    
    std::string getLogFilename() const { return log_filename; }
    bool isLoggingEnabled() const { return is_logging_enabled; }
    
    std::string getCurrentTimestamp() const;
    void flush();
};

#endif // LOGGER_H