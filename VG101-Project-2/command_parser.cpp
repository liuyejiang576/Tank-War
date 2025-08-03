// command_parser.cpp

#include "command_parser.h"
#include <iostream>
#include <getopt.h>
#include <cstring>

CommandParser::CommandParser() {
    setDefaultConfig();
}

CommandParser::~CommandParser() {}

bool CommandParser::parseArguments(int argc, char* argv[]) {
    program_name = argv[0];
    
    static struct option long_options[] = {
        {"help", no_argument, 0, 'h'},
        {"log-file", required_argument, 0, 'l'},
        {"mode", required_argument, 0, 'm'},
        {"initial-life", required_argument, 0, 'p'},
        {0, 0, 0, 0}
    };
    
    int option_index = 0;
    int c;
    
    while ((c = getopt_long(argc, argv, "hl:m:p:", long_options, &option_index)) != -1) {
        switch (c) {
            case 'h':
                config.show_help = true;
                return true;
                
            case 'l':
                config.log_filename = optarg;
                break;
                
            case 'm': {
                GameMode mode = stringToGameMode(optarg);
                if (mode == PVP || mode == PVE || mode == DEMO) {
                    config.mode = mode;
                } else {
                    printError("Invalid game mode: " + std::string(optarg));
                    config.valid_config = false;
                    return false;
                }
                break;
            }
            
            case 'p': {
                int life_points = std::atoi(optarg);
                if (isValidLifePoints(life_points)) {
                    config.initial_life_points = life_points;
                } else {
                    printError("Invalid life points: " + std::string(optarg));
                    config.valid_config = false;
                    return false;
                }
                break;
            }
            
            case '?':
                // getopt_long has printed the error info
                config.valid_config = false;
                return false;
                
            default:
                config.valid_config = false;
                return false;
        }
    }
    
    return validateConfig();
}

void CommandParser::printHelp() const {
    std::cout << "Usage: " << program_name << " [options]\n";
    std::cout << "Options:\n";
    std::cout << "  -h | --help                          Print this help message and exit.\n";
    std::cout << "  --log-file <file>                    Log the game process to a file. (Default: tankwar.log)\n";
    std::cout << "  -m <mode> | --mode=<mode>            Specify the game mode (PVP/PVE/DEMO). (Default: PVP)\n";
    std::cout << "  -p <point> | --initial-life=<point>  Specify the initial life points of the tanks. (Default: 5)\n";
    std::cout << std::endl;
}

void CommandParser::printUsage() const {
    std::cout << "Usage: " << program_name << " [options]\n";
    std::cout << "Try '" << program_name << " --help' for more information.\n";
}

bool CommandParser::validateConfig() const {
    if (!config.valid_config) return false;
    if (config.mode != PVP && config.mode != PVE && config.mode != DEMO) return false;
    if (!isValidLifePoints(config.initial_life_points)) return false;
    if (config.log_filename.empty()) return false;
    
    return true;
}

std::string CommandParser::gameModeToString(GameMode mode) const {
    switch (mode) {
        case PVP:  return "PVP";
        case PVE:  return "PVE";
        case DEMO: return "DEMO";
        default:   return "UNKNOWN";
    }
}

GameMode CommandParser::stringToGameMode(const std::string& mode_str) const {
    if (mode_str == "PVP" || mode_str == "pvp") return PVP;
    else if (mode_str == "PVE" || mode_str == "pve") return PVE;
    else if (mode_str == "DEMO" || mode_str == "demo") return DEMO;

    return PVP; 
}

void CommandParser::setDefaultConfig() {
    config.mode = PVP;
    config.initial_life_points = DEFAULT_LIFE_POINTS;
    config.log_filename = "tankwar.log";
    config.show_help = false;
    config.valid_config = true;
}

bool CommandParser::isValidMode(const std::string& mode_str) const {
    return mode_str == "PVP" || mode_str == "pvp" ||
           mode_str == "PVE" || mode_str == "pve" ||
           mode_str == "DEMO" || mode_str == "demo";
}

bool CommandParser::isValidLifePoints(int points) const {
    return points > 0 && points <= 100; 
}

void CommandParser::printError(const std::string& error_message) const {
    std::cerr << program_name << ": " << error_message << std::endl;
    std::cerr << "Try '" << program_name << " --help' for more information." << std::endl;
}