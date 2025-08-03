#ifndef AI_PLAYER_H
#define AI_PLAYER_H
#include "common.h"
#include <vector>

class GameEngine; 
class Tank;
class Bullet;

struct Position {
    int x, y;
    Position(int x = 0, int y = 0) : x(x), y(y) {}
    bool operator==(const Position& other) const {
        return x == other.x && y == other.y;
    }
};

struct MapBounds {
    int min_x;
    int max_x;
    int min_y;
    int max_y;
    MapBounds(int min_x=0, int max_x=0, int min_y=0, int max_y=0) 
        : min_x(min_x), max_x(max_x), min_y(min_y), max_y(max_y) {}
};

struct AIState {
    Position my_pos;
    Position enemy_pos;
    Direction my_dir;
    Direction enemy_dir;
    int my_life;
    int enemy_life;
    std::vector<Position> bullets;
    int map_size;
    int turn_count;
    MapBounds current_bounds;  
    MapBounds future_bounds;   
    int center_x;
    int center_y;
    int shrink_interval;       
};

class AIPlayer {
private:
    char ai_id;
    int difficulty_level;
    std::vector<Move> move_history;
    static const int SAFE_BORDER = 3;  
    static const int FUTURE_TURNS = 3; 
    int edge_linger_turns;  // to move away from edge

public:
    AIPlayer(char tank_id, int difficulty = 2);
    ~AIPlayer();
    Move makeDecision(const GameEngine& game);
    
    Move makeRandomMove();
    Move makeDefensiveMove(const AIState& state);
    Move makeAggressiveMove(const AIState& state);
    Move makeBalancedMove(const AIState& state);
    
    // evaluate danger
    bool isInDanger(const AIState& state) const;
    bool willBeHitByBullet(const AIState& state, const Position& next_pos) const;
    bool willBeOutOfMap(const MapBounds& bounds, const Position& next_pos) const;
    bool isNearMapEdge(const MapBounds& bounds, const Position& pos) const;
    bool willBeInFutureDanger(const AIState& state, const Position& pos) const;
    
    // decide where to move
    Move findBestEscapeMove(const AIState& state);
    Move findBestAttackMove(const AIState& state) const;
    Move moveTowardsTarget(const Position& current, const Position& target, Direction current_dir) const;
    Move moveAwayFromDanger(const Position& current, const std::vector<Position>& dangers, 
                           Direction current_dir, const AIState& state) const;
    Move moveTowardsCenter(const Position& current, Direction current_dir, const AIState& state) const;
    
    AIState getGameState(const GameEngine& game) const;
    Position getNextPosition(const Position& current, Direction dir, Move move) const;
    Direction getNextDirection(Direction current_dir, Move move) const;
    int calculateDistance(const Position& a, const Position& b) const;
    int evaluatePosition(const AIState& state, const Position& pos) const;
    
    char getAIId() const { return ai_id; }
    int getDifficultyLevel() const { return difficulty_level; }
    void setDifficultyLevel(int level);
    
    void recordMove(Move move);
    void clearHistory();
    bool isRepeatingMoves() const;

private:
    std::vector<Move> getAllPossibleMoves() const;
    Move selectBestMove(const std::vector<Move>& moves, const AIState& state) const;
    int scoreMove(const AIState& state, Move move) const;
    bool canShootEnemy(const AIState& state) const;
    bool isSafePosition(const AIState& state, const Position& pos) const;
    MapBounds predictFutureBounds(const GameEngine& game, int future_turns) const;
};
#endif // AI_PLAYER_H