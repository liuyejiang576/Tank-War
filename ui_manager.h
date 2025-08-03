// ui_manager.h

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include <string>
#include <vector>
#include "common.h"

class GameEngine; 
class Tank;
class Bullet;

class UIManager {
private:
    bool show_detailed_output;
    std::string last_output;

public:
    UIManager(bool detailed = true);
    ~UIManager();
    
    // regular
    void printGameMap(const GameEngine& game) const;
    void printGameStatus(const GameEngine& game) const;
    void printTurnInfo(int turn, char current_player) const;
    void printGameResult(GameResult result) const;
    
    // initialize
    void printWelcomeMessage(GameMode mode) const;
    void printGameRules() const;
    void printInitialSetup(GameMode mode) const;
    
    // input
    Move getPlayerInput(char tank_id) const;
    bool getTankInitialPosition(char tank_id, int& x, int& y, Direction& dir) const;
    bool confirmAction(const std::string& prompt) const;
    
    // output
    void printMessage(const std::string& message) const;
    void printError(const std::string& error) const;
    void printWarning(const std::string& warning) const;
    void clearScreen() const;
    
    void setDetailedOutput(bool detailed) { show_detailed_output = detailed; }
    bool isDetailedOutput() const { return show_detailed_output; }
    
    std::string directionToString(Direction dir) const;
    std::string moveToString(Move move) const;
    std::string gameModeToString(GameMode mode) const;
    
private:
    void printMapBorder(int map_size) const;
    void printMapRow(const GameEngine& game, int row) const;
    char getMapCell(const GameEngine& game, int x, int y) const;
    bool isValidInput(const std::string& input) const;
    Move parseMove(const std::string& input) const;
    Direction parseDirection(const std::string& input) const;
    void printInputPrompt(const std::string& prompt) const;
};

#endif // UI_MANAGER_H