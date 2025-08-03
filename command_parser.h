// command_parser.h

#ifndef COMMAND_PARSER_H
#define COMMAND_PARSER_H

#include <string>
#include "common.h"

struct GameConfig {
    GameMode mode;
    int initial_life_points;
    std::string log_filename;
    bool show_help;
    bool valid_config;
    
    GameConfig() : 
        mode(PVP), 
        initial_life_points(DEFAULT_LIFE_POINTS),
        log_filename("tankwar.log"),
        show_help(false),
        valid_config(true) {}
};

class CommandParser {
private:
    GameConfig config;
    std::string program_name;

public:
    CommandParser();
    ~CommandParser();
    
    bool parseArguments(int argc, char* argv[]);
    
    void printHelp() const;
    void printUsage() const;
    
    bool validateConfig() const;
    
    const GameConfig& getConfig() const { return config; }
    GameMode getGameMode() const { return config.mode; }
    int getInitialLifePoints() const { return config.initial_life_points; }
    std::string getLogFilename() const { return config.log_filename; }
    bool shouldShowHelp() const { return config.show_help; }
    bool isConfigValid() const { return config.valid_config; }
    
    std::string gameModeToString(GameMode mode) const;
    GameMode stringToGameMode(const std::string& mode_str) const;
    
private:
    void setDefaultConfig();
    bool isValidMode(const std::string& mode_str) const;
    bool isValidLifePoints(int points) const;
    void printError(const std::string& error_message) const;
};

#endif // COMMAND_PARSER_H