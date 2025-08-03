// game_engine.cpp

#include "game_engine.h"
#include <iostream>
#include <algorithm>

GameEngine::GameEngine(GameMode mode, int life_points, const std::string& log_file)
    : current_mode(mode), initial_life_points(life_points), 
      current_turn(0), game_result(GAME_CONTINUE), 
      game_running(false), current_player('A') {
    
    initializeComponents();
    logger = std::make_unique<Logger>(log_file);
    ui_manager = std::make_unique<UIManager>(true);
}

GameEngine::~GameEngine() {}

bool GameEngine::initializeGame() {
    try {
        game_map = std::make_unique<GameMap>(INITIAL_MAP_SIZE);
        
        if (!setupTanks()) return false;
        
        if (!setupAI()) return false;
        
        logger->logGameStart(
            (current_mode == PVP) ? "PVP" : 
            (current_mode == PVE) ? "PVE" : "DEMO", 
            initial_life_points
        );
        
        game_running = true;
        return true;
        
    } catch (const std::exception& e) {
        logger->logError("Failed to initialize game: " + std::string(e.what()));
        return false;
    }
}

bool GameEngine::setupTanks() {
    int x_a, y_a, x_b, y_b;
    Direction dir_a, dir_b;
    
    if (current_mode == PVP || current_mode == PVE) {
        ui_manager->printInitialSetup(current_mode);
        
        if (!getInitialTankSetup('A', x_a, y_a, dir_a)) {
            ui_manager->printError("Invalid tank A setup");
            return false;
        }
        
        if (current_mode == PVP) {
            if (!getInitialTankSetup('B', x_b, y_b, dir_b)) {
                ui_manager->printError("Invalid tank B setup");
                return false;
            }
        } else {
            // AI sets automatically
            x_b = 19; y_b = 19; dir_b = D_Left;
        }
    } else {
        // AI sets automatically
        x_a = 0; y_a = 0; dir_a = D_Right;
        x_b = 19; y_b = 19; dir_b = D_Left;
    }
    
    tank_a = std::make_unique<Tank>(x_a, y_a, dir_a, initial_life_points, 'A');
    tank_b = std::make_unique<Tank>(x_b, y_b, dir_b, initial_life_points, 'B');
    
    if (!validateTankPosition(x_a, y_a) || !validateTankPosition(x_b, y_b)) {
        ui_manager->printError("Invalid tank positions");
        return false;
    }
    
    if (areTanksColliding()) {
        ui_manager->printError("Tanks cannot start at the same position");
        return false;
    }
    
    return true;
}

bool GameEngine::setupAI() {
    if (current_mode == PVE) {
        ai_player_b = std::make_unique<AIPlayer>('B', 2); 
    } else if (current_mode == DEMO) {
        ai_player_a = std::make_unique<AIPlayer>('A', 2);
        ai_player_b = std::make_unique<AIPlayer>('B', 2);
    }
    return true;
}

void GameEngine::runGame() {
    if (!initializeGame()) {
        ui_manager->printError("Failed to initialize game");
        return;
    }
    
    ui_manager->printWelcomeMessage(current_mode);
    ui_manager->printGameRules();
    
    while (game_running && gameLoop()) {}
    
    endGame();
}

// execute one full round
bool GameEngine::gameLoop() {
    current_turn++;
    game_map->updateTurn();
    if (game_map->shouldShrink()) {
        logger->logMapShrink(game_map->getCurrentSize());
    }
    ui_manager->printTurnInfo(current_turn, 'A'); 
    processTankTurn(getTankA(), 'A');
    ui_manager->printTurnInfo(current_turn, 'B'); 
    processTankTurn(getTankB(), 'B');
    

    if (checkTankCollision()) {
        game_result = checkGameEnd(); 
        game_running = false;
        return false;
    }

    processBulletMovement();
    processCollisions();
    processOutOfMapDamage();
    updateGameState();
    displayGameState();
    
    game_result = checkGameEnd();
    if (game_result != GAME_CONTINUE) {
        game_running = false;
        return false;
    }
    return true;
}

bool GameEngine::processTurn() {
    try {
        Tank& current_tank = getTankById(current_player);
        if (!processTankTurn(current_tank, current_player)) {
            return false;
        }
        
        processBulletMovement();
        processCollisions();
        processOutOfMapDamage();
        updateGameState();
        return true;
    } catch (const std::exception& e) {
        logger->logError("Error processing turn: " + std::string(e.what()));
        return false;
    }
}

bool GameEngine::processTankTurn(Tank& tank, char tank_id) {
    Move move = getPlayerMove(tank_id);
    tank.move(move);
    logger->logTankMove(tank_id, tank.getX(), tank.getY(), 
                       ui_manager->directionToString(tank.getDirection()));
    tank.updateShootCounter();
    if (tank.canShoot()) {
        spawnBullet(tank);
        tank.resetShootCounter();
    }
    return true;
}

void GameEngine::processBulletMovement() {
    for (auto& bullet : bullets) {
        if (bullet->isActive()) {
            // int old_x = bullet->getX();
            // int old_y = bullet->getY();
            bullet->move();
            logger->logBulletMove(bullet->getX(), bullet->getY(),
                                ui_manager->directionToString(bullet->getDirection()));
            if (bullet->isOutOfBounds(INITIAL_MAP_SIZE + 20)) {
                bullet->deactivate();
            }
        }
    }
    cleanupBullets();
}

bool GameEngine::checkBulletPathCollision(const Bullet& bullet, const Tank& tank) const {
    int tank_x = tank.getX();
    int tank_y = tank.getY();
    int bullet_x = bullet.getX();
    int bullet_y = bullet.getY();

    int next_bullet_x, next_bullet_y;
    bullet.getNextPosition(next_bullet_x, next_bullet_y);

    // check if tank is on the straight line of bullet movement
    bool same_x = (bullet_x == tank_x) && (next_bullet_x == tank_x);
    bool same_y = (bullet_y == tank_y) && (next_bullet_y == tank_y);
    if (!same_x && !same_y) return false;

    // check if tank is between current and next bullet position
    if (same_x) {  // y changes
        int min_y = std::min(bullet_y, next_bullet_y);
        int max_y = std::max(bullet_y, next_bullet_y);
        return (tank_y >= min_y && tank_y <= max_y);
    } else {  // x changes
        int min_x = std::min(bullet_x, next_bullet_x);
        int max_x = std::max(bullet_x, next_bullet_x);
        return (tank_x >= min_x && tank_x <= max_x);
    }
}

void GameEngine::processCollisions() {
    for (auto& bullet : bullets) {
        if (!bullet->isActive()) continue;
        
        if (bullet->checkCollisionWithTank(*tank_a)) {
            handleBulletHit(*bullet, *tank_a);
            bullet->deactivate();
            continue;
        }

        if (bullet->checkCollisionWithTank(*tank_b)) {
            handleBulletHit(*bullet, *tank_b);
            bullet->deactivate();
            continue;
        }
    }
}

void GameEngine::processOutOfMapDamage() {
    if (game_map->shouldTakeDamageOutOfMap(*tank_a)) {
        tank_a->takeDamage(OUT_OF_MAP_DAMAGE);
        logger->logTankDamage('A', tank_a->getLifePoints(), "out of map");
    }
    
    if (game_map->shouldTakeDamageOutOfMap(*tank_b)) {
        tank_b->takeDamage(OUT_OF_MAP_DAMAGE);
        logger->logTankDamage('B', tank_b->getLifePoints(), "out of map");
    }
}

GameResult GameEngine::checkGameEnd() {
    bool tank_a_alive = tank_a->isAlive();
    bool tank_b_alive = tank_b->isAlive();

    if (checkTankCollision()) {
        if (tank_a->getLifePoints() > tank_b->getLifePoints()) return TANK_A_WIN;
        else if (tank_b->getLifePoints() > tank_a->getLifePoints()) return TANK_B_WIN;
        else return DRAW;
    }

    if (!tank_a_alive && !tank_b_alive) return DRAW;
    else if (!tank_a_alive) return TANK_B_WIN;
    else if (!tank_b_alive) return TANK_A_WIN;
    
    return GAME_CONTINUE;
}

bool GameEngine::checkTankCollision() {
    return tank_a->getX() == tank_b->getX() && tank_a->getY() == tank_b->getY();
}

void GameEngine::handleBulletHit(Bullet& bullet, Tank& tank) {
    (void)bullet;
    tank.takeDamage(BULLET_DAMAGE);
    logger->logBulletHit(tank.getTankId(), BULLET_DAMAGE);
    logger->logTankDamage(tank.getTankId(), tank.getLifePoints(), "bullet hit");
}

void GameEngine::spawnBullet(Tank& tank) {
    int bullet_x, bullet_y;
    tank.getBulletSpawnPosition(bullet_x, bullet_y);
    
    auto new_bullet = std::make_unique<Bullet>(
        bullet_x, bullet_y, tank.getDirection(), tank.getTankId()
    );
    
    bullets.push_back(std::move(new_bullet));
    
    logger->logTankShoot(tank.getTankId(), bullet_x, bullet_y);
}

Move GameEngine::getPlayerMove(char tank_id) {
    if (current_mode == PVP) return ui_manager->getPlayerInput(tank_id);
    else if (current_mode == PVE) {
        if (tank_id == 'A') return ui_manager->getPlayerInput(tank_id);
        else return getAIMove(tank_id);
    }
    else return getAIMove(tank_id); // DEMO
}

Move GameEngine::getAIMove(char tank_id) {
    if (tank_id == 'A' && ai_player_a) return ai_player_a->makeDecision(*this);
    else if (tank_id == 'B' && ai_player_b) return ai_player_b->makeDecision(*this);
    return M_Forward; // default
}

bool GameEngine::getInitialTankSetup(char tank_id, int& x, int& y, Direction& dir) {
    return ui_manager->getTankInitialPosition(tank_id, x, y, dir);
}

void GameEngine::updateGameState() {
    logGameState();
}

void GameEngine::resetGame() {
    current_turn = 0;
    game_result = GAME_CONTINUE;
    game_running = false;
    current_player = 'A';
    
    bullets.clear();
    game_map->reset();
}

void GameEngine::endGame() {
    ui_manager->printGameResult(game_result);
    
    std::string result_str;
    switch (game_result) {
        case TANK_A_WIN: result_str = "Tank A Wins"; break;
        case TANK_B_WIN: result_str = "Tank B Wins"; break;
        case DRAW: result_str = "Draw"; break;
        default: result_str = "Game ended unexpectedly"; break;
    }
    
    logger->logGameResult(result_str);
}

Tank& GameEngine::getTankById(char tank_id) {
    return (tank_id == 'A') ? *tank_a : *tank_b;
}

const Tank& GameEngine::getTankById(char tank_id) const {
    return (tank_id == 'A') ? *tank_a : *tank_b;
}

Tank& GameEngine::getOtherTank(char tank_id) {
    return (tank_id == 'A') ? *tank_b : *tank_a;
}

const Tank& GameEngine::getOtherTank(char tank_id) const {
    return (tank_id == 'A') ? *tank_b : *tank_a;
}

char GameEngine::getOtherTankId(char tank_id) const {
    return (tank_id == 'A') ? 'B' : 'A';
}

void GameEngine::switchPlayer() {
    current_player = (current_player == 'A') ? 'B' : 'A';
}

void GameEngine::initializeComponents() {
    bullets.clear();
}

void GameEngine::cleanupBullets() {
    bullets.erase(
        std::remove_if(bullets.begin(), bullets.end(),
            [](const std::unique_ptr<Bullet>& bullet) {
                return !bullet->isActive();
            }),
        bullets.end()
    );
}

bool GameEngine::validateTankPosition(int x, int y) const {
    return x >= 0 && x < INITIAL_MAP_SIZE && y >= 0 && y < INITIAL_MAP_SIZE;
}

bool GameEngine::areTanksColliding() const {
    return tank_a->getX() == tank_b->getX() && tank_a->getY() == tank_b->getY();
}

void GameEngine::logGameState() const {
    logger->logTurn(current_turn);
}

void GameEngine::displayGameState() const {
    if (ui_manager) {
        ui_manager->printGameMap(*this);
        if (current_turn % 5 == 0) { // show detailed status every five rounds
            ui_manager->printGameStatus(*this);
        }
    }
}