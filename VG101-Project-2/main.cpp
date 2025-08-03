// main.cpp

#include "game_engine.h"
#include "command_parser.h"
#include <iostream>
#include <memory>

int main(int argc, char* argv[]) {
    try {
        CommandParser parser;
        if (!parser.parseArguments(argc, argv)) {
            std::cerr << "Failed to parse command line arguments." << std::endl;
            return 1;
        }
        
        if (parser.shouldShowHelp()) {
            parser.printHelp();
            return 0;
        }
        
        if (!parser.isConfigValid()) {
            std::cerr << "Invalid configuration." << std::endl;
            parser.printUsage();
            return 1;
        }
        
        const GameConfig& config = parser.getConfig();
        
        auto game_engine = std::make_unique<GameEngine>(
            config.mode,
            config.initial_life_points,
            config.log_filename
        );
        
        game_engine->runGame();
        
        return 0;
        
    } catch (const std::exception& e) {
        std::cerr << "Fatal error: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "Unknown fatal error occurred." << std::endl;
        return 1;
    }
}