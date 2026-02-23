// game_engine.h

#ifndef GAME_ENGINE_H
#define GAME_ENGINE_H

#include <vector>
#include <memory>
#include "common.h"
#include "tank.h"
#include "bullet.h"
#include "game_map.h"
#include "logger.h"
#include "ui_manager.h"
#include "ai_player.h"
#include "landmine.h"

class GameEngine {
private:
    // object
    std::unique_ptr<Tank> tank_a;
    std::unique_ptr<Tank> tank_b;
    std::vector<std::unique_ptr<Bullet>> bullets;
    std::unique_ptr<GameMap> game_map;
    std::unique_ptr<LandmineManager> landmine_manager;
    
    // manage
    std::unique_ptr<Logger> logger;
    std::unique_ptr<UIManager> ui_manager;
    std::unique_ptr<AIPlayer> ai_player_a;
    std::unique_ptr<AIPlayer> ai_player_b;
    
    // status
    GameMode current_mode;
    int initial_life_points;
    int current_turn;
    GameResult game_result;
    bool game_running;
    char current_player;
    
public:
    GameEngine(GameMode mode, int life_points, const std::string& log_file);
    ~GameEngine();
    
    // initialize
    bool initializeGame();
    bool setupTanks();
    bool setupAI();
    
    // main loop
    void runGame();
    bool gameLoop();
    bool processTurn();
    
    bool processTankTurn(Tank& tank, char tank_id);
    void processBulletMovement();
    void processCollisions();
    void processLandmines();
    void updateLandmines();
    // void processMapShrink();
    void processOutOfMapDamage();
    
    // check rules
    GameResult checkGameEnd();
    bool checkTankCollision();
    void handleBulletHit(Bullet& bullet, Tank& tank);
    void spawnBullet(Tank& tank);
    
    // input
    Move getPlayerMove(char tank_id);
    Move getAIMove(char tank_id);
    bool getInitialTankSetup(char tank_id, int& x, int& y, Direction& dir);
    
    void updateGameState();
    void resetGame();
    void endGame();
    
    // get with const for other classes
    const Tank& getTankA() const { return *tank_a; }
    const Tank& getTankB() const { return *tank_b; }
    const std::vector<std::unique_ptr<Bullet>>& getBullets() const { return bullets; }
    const GameMap& getGameMap() const { return *game_map; }
    const LandmineManager& getLandmineManager() const { return *landmine_manager; }
    GameMode getCurrentMode() const { return current_mode; }
    int getCurrentTurn() const { return current_turn; }
    int getInitialLifePoints() const { return initial_life_points; }
    GameResult getGameResult() const { return game_result; }
    bool isGameRunning() const { return game_running; }
    char getCurrentPlayer() const { return current_player; }
    
    // get without const for internal classes
    Tank& getTankA() { return *tank_a; }
    Tank& getTankB() { return *tank_b; }
    std::vector<std::unique_ptr<Bullet>>& getBullets() { return bullets; }
    GameMap& getGameMap() { return *game_map; }
    Logger& getLogger() { return *logger; }
    UIManager& getUIManager() { return *ui_manager; }

    Tank& getTankById(char tank_id);
    const Tank& getTankById(char tank_id) const;
    Tank& getOtherTank(char tank_id);
    const Tank& getOtherTank(char tank_id) const;
    char getOtherTankId(char tank_id) const;
    void switchPlayer();
    
    // void printDebugInfo() const;
    // bool isValidGameState() const;

private:
    void initializeComponents();
    void cleanupBullets();
    bool validateTankPosition(int x, int y) const;
    bool areTanksColliding() const;
    void logGameState() const;
    void displayGameState() const;
    bool checkBulletPathCollision(const Bullet& bullet, const Tank& tank) const;  
};

#endif // GAME_ENGINE_H