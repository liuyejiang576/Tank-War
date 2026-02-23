// ui_manager.cpp

#include "ui_manager.h"
#include "game_engine.h"
#include "tank.h"
#include "bullet.h"
#include "game_map.h"
#include <iostream>
#include <iomanip>
#include <sstream>

UIManager::UIManager(bool detailed) : show_detailed_output(detailed) {
}

UIManager::~UIManager() {}

void UIManager::printGameMap(const GameEngine& game) const {
    const GameMap& map = game.getGameMap();
    const Tank& tank_a = game.getTankA();
    const Tank& tank_b = game.getTankB();
    // const auto& bullets = game.getBullets();
    
    int map_size = map.getCurrentSize();
    // int center = map_size / 2;
    
    std::cout << "A: " << tank_a.getLifePoints() 
              << ", B: " << tank_b.getLifePoints() 
              << ", Turn: " << game.getCurrentTurn() << std::endl;

    printMapBorder(map_size);
    
    // print map contents
    for (int y = map.getMinY() - 3; y <= map.getMaxY() + 3; y++) {
        std::cout << "|";
        for (int x = map.getMinX() - 3; x <= map.getMaxX() + 3; x++) {
            char cell = getMapCell(game, x, y);
            std::cout << cell << "|";
        }
        std::cout << std::endl;
    }
    
    printMapBorder(map_size);
}

void UIManager::printGameStatus(const GameEngine& game) const {
    const Tank& tank_a = game.getTankA();
    const Tank& tank_b = game.getTankB();
    
    std::cout << "\n=== Game Status ===" << std::endl;
    std::cout << "Tank A: Life=" << tank_a.getLifePoints() 
              << ", Position=(" << tank_a.getX() << "," << tank_a.getY() << ")"
              << ", Direction=" << directionToString(tank_a.getDirection()) << std::endl;
    std::cout << "Tank B: Life=" << tank_b.getLifePoints() 
              << ", Position=(" << tank_b.getX() << "," << tank_b.getY() << ")"
              << ", Direction=" << directionToString(tank_b.getDirection()) << std::endl;
    std::cout << "Map Size: " << game.getGameMap().getCurrentSize() << "x" 
              << game.getGameMap().getCurrentSize() << std::endl;
    std::cout << "Turn: " << game.getCurrentTurn() << std::endl;
    std::cout << "===================" << std::endl;
}

void UIManager::printTurnInfo(int turn, char current_player) const {
    std::cout << "\n--- Turn " << turn << " ---" << std::endl;
    std::cout << "Player " << current_player << "'s turn" << std::endl;
}

void UIManager::printGameResult(GameResult result) const {
    std::cout << "\n=== GAME OVER ===" << std::endl;
    switch (result) {
        case TANK_A_WIN:
            std::cout << "Tank A Wins!" << std::endl;
            break;
        case TANK_B_WIN:
            std::cout << "Tank B Wins!" << std::endl;
            break;
        case DRAW:
            std::cout << "Draw!" << std::endl;
            break;
        default:
            std::cout << "Game ended unexpectedly." << std::endl;
            break;
    }
    std::cout << "=================" << std::endl;
}

void UIManager::printWelcomeMessage(GameMode mode) const {
    std::cout << "=== Tank War Game ===" << std::endl;
    std::cout << "Mode: " << gameModeToString(mode) << std::endl;
    std::cout << "======================" << std::endl << std::endl;
}

void UIManager::printGameRules() const {
    std::cout << "Game Rules:" << std::endl;
    std::cout << "- Each tank has initial life points" << std::endl;
    std::cout << "- Tanks move 1 meter per turn" << std::endl;
    std::cout << "- Bullets are shot every 3 turns" << std::endl;
    std::cout << "- Bullets move 2 meters per turn" << std::endl;
    std::cout << "- Map shrinks every 16 turns" << std::endl;
    std::cout << "- Being outside map costs 1 life per turn" << std::endl;
    std::cout << std::endl;
}

void UIManager::printInitialSetup(GameMode mode) const {
    if (mode == PVP || mode == PVE) {
        std::cout << "Please set up initial tank positions:" << std::endl;
    } else {
        std::cout << "AI tanks will be positioned automatically." << std::endl;
    }
}

Move UIManager::getPlayerInput(char tank_id) const {
    std::cout << "Tank " << tank_id << " - Enter move (0=Forward, 1=Left, 2=Right): ";
    
    int input;
    while (!(std::cin >> input) || input < 0 || input > 2) {
        std::cout << "Invalid input! Please enter 0, 1, or 2: ";
        std::cin.clear();
        std::cin.ignore(10000, '\n');
    }
    
    return static_cast<Move>(input);
}

bool UIManager::getTankInitialPosition(char tank_id, int& x, int& y, Direction& dir) const {
    std::cout << "Set initial position for Tank " << tank_id << ":" << std::endl;
    
    std::cout << "Enter X coordinate: ";
    if (!(std::cin >> x)) {
        return false;
    }
    
    std::cout << "Enter Y coordinate: ";
    if (!(std::cin >> y)) {
        return false;
    }
    
    std::cout << "Enter direction (0=Left, 1=Up, 2=Right, 3=Down): ";
    int dir_input;
    if (!(std::cin >> dir_input) || dir_input < 0 || dir_input > 3) {
        return false;
    }
    
    dir = static_cast<Direction>(dir_input);
    return true;
}

bool UIManager::confirmAction(const std::string& prompt) const {
    std::cout << prompt << " (y/n): ";
    char response;
    std::cin >> response;
    return response == 'y' || response == 'Y';
}

void UIManager::printMessage(const std::string& message) const {
    std::cout << message << std::endl;
}

void UIManager::printError(const std::string& error) const {
    std::cerr << "Error: " << error << std::endl;
}

void UIManager::printWarning(const std::string& warning) const {
    std::cout << "Warning: " << warning << std::endl;
}

void UIManager::clearScreen() const {
    // clear
    std::cout << "\033[2J\033[1;1H";
}

std::string UIManager::directionToString(Direction dir) const {
    switch (dir) {
        case D_Left:  return "Left";
        case D_Up:    return "Up";
        case D_Right: return "Right";
        case D_Down:  return "Down";
        default:      return "Unknown";
    }
}

std::string UIManager::moveToString(Move move) const {
    switch (move) {
        case M_Forward: return "Forward";
        case M_Left:    return "Left";
        case M_Right:   return "Right";
        default:        return "Unknown";
    }
}

std::string UIManager::gameModeToString(GameMode mode) const {
    switch (mode) {
        case PVP:  return "Player vs Player";
        case PVE:  return "Player vs Environment";
        case DEMO: return "Demo (AI vs AI)";
        default:   return "Unknown";
    }
}

void UIManager::printMapBorder(int map_size) const {
    std::cout << "-";
    for (int i = 0; i < (map_size + 6) * 2 + 1; i++) {
        std::cout << "-";
    }
    std::cout << std::endl;
}

char UIManager::getMapCell(const GameEngine& game, int x, int y) const {
    const Tank& tank_a = game.getTankA();
    const Tank& tank_b = game.getTankB();
    const auto& bullets = game.getBullets();
    const GameMap& map = game.getGameMap();
    
    // Show tanks with direction indicators
    if (tank_a.isAtPosition(x, y)) {
        return getDirectionChar('A', tank_a.getDirection());
    }
    if (tank_b.isAtPosition(x, y)) {
        return getDirectionChar('B', tank_b.getDirection());
    }
    
    // Show bullets with direction indicators
    for (const auto& bullet : bullets) {
        if (bullet->isActive() && bullet->isAtPosition(x, y)) {
            return getBulletDirectionChar(bullet->getDirection());
        }
    }

    if (map.isInBounds(x, y)) return ' ';
    else return '-';
}

char UIManager::getDirectionChar(char tank_id, Direction dir) const {
    // Use arrows to indicate tank direction
    // Tank A: lowercase arrows, Tank B: uppercase/symbols
    if (tank_id == 'A') {
        switch (dir) {
            case D_Left:  return '<';   // A facing left
            case D_Up:    return '^';   // A facing up
            case D_Right: return '>';   // A facing right
            case D_Down:  return 'v';   // A facing down
        }
    } else {
        switch (dir) {
            case D_Left:  return 'L';   // B facing left
            case D_Up:    return 'U';   // B facing up  
            case D_Right: return 'R';   // B facing right
            case D_Down:  return 'D';   // B facing down
        }
    }
    return tank_id;
}

char UIManager::getBulletDirectionChar(Direction dir) const {
    // Use simple ASCII characters for bullet directions (Windows compatible)
    switch (dir) {
        case D_Left:  return '-';  // horizontal bullet
        case D_Up:    return '|';  // vertical bullet  
        case D_Right: return '-';  // horizontal bullet
        case D_Down:  return '|';  // vertical bullet
    }
    return '*';
}

void UIManager::printMapRow(const GameEngine& game, int row) const {
    const GameMap& map = game.getGameMap();
    int min_x = map.getMinX() - 3; 
    int max_x = map.getMaxX() + 3;
    std::cout << "|";
    for (int x = min_x; x <= max_x; x++) {
        std::cout << getMapCell(game, x, row) << "|";
    }
    std::cout << std::endl;
}