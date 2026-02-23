#include "ai_player.h"
#include "game_engine.h"
#include "tank.h"
#include "bullet.h"
#include "game_map.h"
#include <random>
#include <algorithm>
#include <climits>

AIPlayer::AIPlayer(char tank_id, int difficulty) 
    : ai_id(tank_id), difficulty_level(difficulty), edge_linger_turns(0) {
    if (difficulty_level < 1) difficulty_level = 1;
    if (difficulty_level > 3) difficulty_level = 3;
}

AIPlayer::~AIPlayer() {}

Move AIPlayer::makeDecision(const GameEngine& game) {
    AIState state = getGameState(game);
    Move chosen_move;

    // move away from edge
    if (edge_linger_turns > 0) {
        chosen_move = moveTowardsCenter(state.my_pos, state.my_dir, state);
        edge_linger_turns--;
        recordMove(chosen_move);
        return chosen_move;
    }

    switch (difficulty_level) {
        case 1: 
            chosen_move = makeRandomMove(); 
            if (willBeOutOfMap(state.current_bounds, getNextPosition(state.my_pos, state.my_dir, chosen_move))) {
                chosen_move = moveTowardsCenter(state.my_pos, state.my_dir, state);
            }
            break;
        case 2: chosen_move = makeBalancedMove(state); break;
        case 3: chosen_move = makeAggressiveMove(state); break;
        default: chosen_move = makeBalancedMove(state); break;
    }

    if (isNearMapEdge(state.current_bounds, getNextPosition(state.my_pos, state.my_dir, chosen_move))) {
        edge_linger_turns = 2;  // leave the border in time
    }

    recordMove(chosen_move);
    return chosen_move;
}

Move AIPlayer::makeRandomMove() {
    static std::random_device rd;
    static std::mt19937 gen(rd());
    static std::uniform_int_distribution<> dis(0, 2);
    return static_cast<Move>(dis(gen));
}

Move AIPlayer::makeDefensiveMove(const AIState& state) {
    if (isInDanger(state)) {
        return findBestEscapeMove(state);
    }

    // incline towards safe places away from the border
    std::vector<Move> safe_moves;
    for (Move move : getAllPossibleMoves()) {
        Position next_pos = getNextPosition(state.my_pos, state.my_dir, move);
        if (isSafePosition(state, next_pos) && !willBeInFutureDanger(state, next_pos)) {
            safe_moves.push_back(move);
        }
    }

    if (!safe_moves.empty()) {
        return selectBestMove(safe_moves, state);
    }

    // default
    return moveTowardsCenter(state.my_pos, state.my_dir, state);
}

Move AIPlayer::makeAggressiveMove(const AIState& state) {
    if (isInDanger(state)) {
        return findBestEscapeMove(state);
    }

    Move attack_move = findBestAttackMove(state);
    Position attack_pos = getNextPosition(state.my_pos, state.my_dir, attack_move);
    if (canShootEnemy(state) && isSafePosition(state, attack_pos) && !willBeInFutureDanger(state, attack_pos)) {
        return attack_move;
    }

    std::vector<Move> valid_moves;
    for (Move move : getAllPossibleMoves()) {
        Position next_pos = getNextPosition(state.my_pos, state.my_dir, move);
        if (isSafePosition(state, next_pos) && !willBeInFutureDanger(state, next_pos)) {
            valid_moves.push_back(move);
        }
    }

    if (!valid_moves.empty()) {
        return selectBestMove(valid_moves, state);
    }

    return moveTowardsCenter(state.my_pos, state.my_dir, state);
}

Move AIPlayer::makeBalancedMove(const AIState& state) {
    // Priority 1: Immediate danger avoidance
    if (isInDanger(state)) {
        return findBestEscapeMove(state);
    }

    // Priority 2: Predict and avoid incoming bullets
    Move dodge_move = findDodgeMove(state);
    if (dodge_move != M_Forward || isInBulletPath(state)) {
        Position dodge_pos = getNextPosition(state.my_pos, state.my_dir, dodge_move);
        if (!willBeOutOfMap(state.future_bounds, dodge_pos)) {
            return dodge_move;
        }
    }

    // Priority 3: Attack if possible
    if (canShootEnemy(state)) {
        Move attack_move = findBestAttackMove(state);
        Position attack_pos = getNextPosition(state.my_pos, state.my_dir, attack_move);
        if (isSafePosition(state, attack_pos) && !willBeInFutureDanger(state, attack_pos)) {
            return attack_move;
        }
    }

    // Priority 4: Position for better attack angle
    Move position_move = findPositioningMove(state);
    Position pos_pos = getNextPosition(state.my_pos, state.my_dir, position_move);
    if (isSafePosition(state, pos_pos) && !willBeInFutureDanger(state, pos_pos)) {
        return position_move;
    }

    // Priority 5: Move to safe position near center
    std::vector<Move> good_moves;
    for (Move move : getAllPossibleMoves()) {
        Position next_pos = getNextPosition(state.my_pos, state.my_dir, move);
        if (isSafePosition(state, next_pos) && !willBeInFutureDanger(state, next_pos)) {
            good_moves.push_back(move);
        }
    }

    if (!good_moves.empty()) {
        return selectBestMove(good_moves, state);
    }

    return moveTowardsCenter(state.my_pos, state.my_dir, state);
}

bool AIPlayer::isInDanger(const AIState& state) const {
    return !isSafePosition(state, state.my_pos) || willBeInFutureDanger(state, state.my_pos);
}

bool AIPlayer::willBeHitByBullet(const AIState& state, const Position& next_pos) const {
    for (const Position& bullet_pos : state.bullets) {
        int dx = abs(bullet_pos.x - next_pos.x);
        int dy = abs(bullet_pos.y - next_pos.y);
        if ((dx <= BULLET_SPEED && dy == 0) || (dy <= BULLET_SPEED && dx == 0)) {
            return true;
        }
    }
    return false;
}

bool AIPlayer::willBeOutOfMap(const MapBounds& bounds, const Position& next_pos) const {
    return next_pos.x < bounds.min_x || next_pos.x > bounds.max_x ||
           next_pos.y < bounds.min_y || next_pos.y > bounds.max_y;
}

bool AIPlayer::isNearMapEdge(const MapBounds& bounds, const Position& pos) const {
    return (pos.x - bounds.min_x) <= SAFE_BORDER || 
           (bounds.max_x - pos.x) <= SAFE_BORDER ||
           (pos.y - bounds.min_y) <= SAFE_BORDER || 
           (bounds.max_y - pos.y) <= SAFE_BORDER;
}

bool AIPlayer::willBeInFutureDanger(const AIState& state, const Position& pos) const {
    return isNearMapEdge(state.future_bounds, pos) || 
           willBeOutOfMap(state.future_bounds, pos);
}

Move AIPlayer::findBestEscapeMove(const AIState& state) {
    std::vector<Position> dangers = state.bullets;

    if (isNearMapEdge(state.current_bounds, state.my_pos)) {
        dangers.emplace_back(state.current_bounds.min_x, state.my_pos.y);
        dangers.emplace_back(state.current_bounds.max_x, state.my_pos.y);
        dangers.emplace_back(state.my_pos.x, state.current_bounds.min_y);
        dangers.emplace_back(state.my_pos.x, state.current_bounds.max_y);
    }
    if (willBeInFutureDanger(state, state.my_pos)) {
        dangers.emplace_back(state.future_bounds.min_x, state.my_pos.y);
        dangers.emplace_back(state.future_bounds.max_x, state.my_pos.y);
        dangers.emplace_back(state.my_pos.x, state.future_bounds.min_y);
        dangers.emplace_back(state.my_pos.x, state.future_bounds.max_y);
    }

    return moveAwayFromDanger(state.my_pos, dangers, state.my_dir, state);
}

Move AIPlayer::findBestAttackMove(const AIState& state) const {
    return moveTowardsTarget(state.my_pos, state.enemy_pos, state.my_dir);
}

Move AIPlayer::moveTowardsTarget(const Position& current, const Position& target, Direction current_dir) const {
    int dx = target.x - current.x;
    int dy = target.y - current.y;

    Direction desired_dir;
    if (abs(dx) > abs(dy)) {
        desired_dir = (dx > 0) ? D_Right : D_Left;
    } else {
        desired_dir = (dy > 0) ? D_Down : D_Up;
    }

    if (current_dir == desired_dir) {
        return M_Forward;
    } else if (turnLeft(current_dir) == desired_dir) {
        return M_Left;
    } else if (turnRight(current_dir) == desired_dir) {
        return M_Right;
    }

    return M_Forward;
}

Move AIPlayer::moveTowardsCenter(const Position& current, Direction current_dir, const AIState& state) const {
    Position center(state.center_x, state.center_y);
    return moveTowardsTarget(current, center, current_dir);
}

Move AIPlayer::moveAwayFromDanger(const Position& current, const std::vector<Position>& dangers, 
                                 Direction current_dir, const AIState& state) const {
    std::vector<Move> moves = getAllPossibleMoves();
    int best_score = INT_MIN;
    Move best_move = M_Forward;

    for (Move move : moves) {
        Position next_pos = getNextPosition(current, current_dir, move);
        if (willBeOutOfMap(state.future_bounds, next_pos)) continue;
        if (willBeOutOfMap(state.current_bounds, next_pos)) continue;

        int score = 0;
        for (const Position& danger : dangers) {
            score += calculateDistance(next_pos, danger) * 2;  // far from danger: 2 points
        }
        score -= calculateDistance(next_pos, Position(state.center_x, state.center_y)) * 3;  // near center: 3 points

        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
    }

    return best_move;
}

AIState AIPlayer::getGameState(const GameEngine& game) const {
    AIState state;
    const Tank& my_tank = game.getTankById(ai_id);
    const Tank& enemy_tank = game.getOtherTank(ai_id);
    const GameMap& map = game.getGameMap();

    state.my_pos = Position(my_tank.getX(), my_tank.getY());
    state.enemy_pos = Position(enemy_tank.getX(), enemy_tank.getY());
    state.my_dir = my_tank.getDirection();
    state.enemy_dir = enemy_tank.getDirection();
    state.my_life = my_tank.getLifePoints();
    state.enemy_life = enemy_tank.getLifePoints();
    state.map_size = map.getCurrentSize();
    state.turn_count = game.getCurrentTurn();
    state.center_x = map.getMapCenter();
    state.center_y = map.getMapCenter();
    state.shrink_interval = MAP_SHRINK_INTERVAL;

    state.current_bounds = MapBounds(
        map.getMinX(),
        map.getMaxX(),
        map.getMinY(),
        map.getMaxY()
    );

    state.future_bounds = predictFutureBounds(game, FUTURE_TURNS);

    for (const auto& bullet : game.getBullets()) {
        if (bullet->isActive()) {
            state.bullets.push_back(Position(bullet->getX(), bullet->getY()));
        }
    }

    return state;
}

MapBounds AIPlayer::predictFutureBounds(const GameEngine& game, int future_turns) const {
    const GameMap& current_map = game.getGameMap();
    int current_size = current_map.getCurrentSize();
    int current_turn = game.getCurrentTurn();
    int shrink_count = 0;

    for (int i = 1; i <= future_turns; i++)
        if ((current_turn + i) % MAP_SHRINK_INTERVAL == 0) shrink_count++;

    int future_size = current_size - (shrink_count * 2);
    if (future_size < 2) future_size = 2;  

    int center = current_map.getMapCenter();
    int half_size = future_size / 2;
    return MapBounds(
        center - half_size,
        center + half_size - 1,
        center - half_size,
        center + half_size - 1
    );
}

Position AIPlayer::getNextPosition(const Position& current, Direction dir, Move move) const {
    Direction next_dir = getNextDirection(dir, move);
    Position next_pos = current;

    if (move == M_Forward) {
        switch (next_dir) {
            case D_Left: next_pos.x -= TANK_SPEED; break;
            case D_Up: next_pos.y -= TANK_SPEED; break;
            case D_Right: next_pos.x += TANK_SPEED; break;
            case D_Down: next_pos.y += TANK_SPEED; break;
        }
    }

    return next_pos;
}

Direction AIPlayer::getNextDirection(Direction current_dir, Move move) const {
    switch (move) {
        case M_Forward: return current_dir;
        case M_Left: return turnLeft(current_dir);
        case M_Right: return turnRight(current_dir);
        default: return current_dir;
    }
}

int AIPlayer::calculateDistance(const Position& a, const Position& b) const {
    return abs(a.x - b.x) + abs(a.y - b.y);
}

int AIPlayer::evaluatePosition(const AIState& state, const Position& pos) const {
    int score = 0;
    score += calculateDistance(pos, state.enemy_pos) * 1; // 1. far from the enemy
    score -= calculateDistance(pos, Position(state.center_x, state.center_y)) * 4; // 2. near the center

    // 3. far from the bullet
    for (const Position& bullet : state.bullets) {
        int dist = calculateDistance(pos, bullet);
        if (dist < 4) score -= (4 - dist) * 10;
    }

    // 4. far from the current edge
    if (isNearMapEdge(state.current_bounds, pos)) {
        int edge_penalty = 0;
        edge_penalty += (SAFE_BORDER - (pos.x - state.current_bounds.min_x)) * 20;
        edge_penalty += (SAFE_BORDER - (state.current_bounds.max_x - pos.x)) * 20;
        edge_penalty += (SAFE_BORDER - (pos.y - state.current_bounds.min_y)) * 20;
        edge_penalty += (SAFE_BORDER - (state.current_bounds.max_y - pos.y)) * 20;
        score -= edge_penalty;
    }

    // 5. far from the future edge
    if (isNearMapEdge(state.future_bounds, pos)) {
        int future_penalty = 0;
        future_penalty += (SAFE_BORDER - (pos.x - state.future_bounds.min_x)) * 15;
        future_penalty += (SAFE_BORDER - (state.future_bounds.max_x - pos.x)) * 15;
        future_penalty += (SAFE_BORDER - (pos.y - state.future_bounds.min_y)) * 15;
        future_penalty += (SAFE_BORDER - (state.future_bounds.max_y - pos.y)) * 15;
        score -= future_penalty;
    }

    return score;
}

void AIPlayer::setDifficultyLevel(int level) {
    if (level >= 1 && level <= 3) {
        difficulty_level = level;
    }
}

void AIPlayer::recordMove(Move move) {
    move_history.push_back(move);
    if (move_history.size() > 10) {
        move_history.erase(move_history.begin());
    }
}

void AIPlayer::clearHistory() {
    move_history.clear();
}

bool AIPlayer::isRepeatingMoves() const {
    if (move_history.size() < 4) return false;
    size_t recent_size = move_history.size();
    return move_history[recent_size-1] == move_history[recent_size-3] &&
           move_history[recent_size-2] == move_history[recent_size-4];
}

std::vector<Move> AIPlayer::getAllPossibleMoves() const {
    return {M_Forward, M_Left, M_Right};
}

Move AIPlayer::selectBestMove(const std::vector<Move>& moves, const AIState& state) const {
    if (moves.empty()) return M_Forward;

    int best_score = INT_MIN;
    Move best_move = moves[0];

    for (Move move : moves) {
        int score = scoreMove(state, move);
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
    }

    return best_move;
}

int AIPlayer::scoreMove(const AIState& state, Move move) const {
    Position next_pos = getNextPosition(state.my_pos, state.my_dir, move);
    return evaluatePosition(state, next_pos);
}

bool AIPlayer::canShootEnemy(const AIState& state) const {
    int dx = abs(state.enemy_pos.x - state.my_pos.x);
    int dy = abs(state.enemy_pos.y - state.my_pos.y);

    switch (state.my_dir) {
        case D_Left:  return dy == 0 && state.enemy_pos.x < state.my_pos.x;
        case D_Up:    return dx == 0 && state.enemy_pos.y < state.my_pos.y;
        case D_Right: return dy == 0 && state.enemy_pos.x > state.my_pos.x;
        case D_Down:  return dx == 0 && state.enemy_pos.y > state.my_pos.y;
    }
    return false;
}

bool AIPlayer::isSafePosition(const AIState& state, const Position& pos) const {
    if (willBeOutOfMap(state.current_bounds, pos)) return false;
    if (willBeHitByBullet(state, pos)) return false;
    if (difficulty_level >= 2 && isNearMapEdge(state.current_bounds, pos)) return false;
    return true;
}

// New smarter AI functions

bool AIPlayer::isInBulletPath(const AIState& state) const {
    // Check if current position is in the path of any bullet
    for (const Position& bullet : state.bullets) {
        // Check if bullet is moving towards us (same row or column)
        if (bullet.x == state.my_pos.x || bullet.y == state.my_pos.y) {
            int dist = calculateDistance(bullet, state.my_pos);
            if (dist <= BULLET_SPEED * 2) {  // Within 2 turns of bullet travel
                return true;
            }
        }
    }
    return false;
}

Move AIPlayer::findDodgeMove(const AIState& state) const {
    // Find the best move to dodge incoming bullets
    std::vector<Move> moves = getAllPossibleMoves();
    int best_score = INT_MIN;
    Move best_move = M_Forward;
    
    for (Move move : moves) {
        Position next_pos = getNextPosition(state.my_pos, state.my_dir, move);
        
        // Skip if move goes out of bounds
        if (willBeOutOfMap(state.current_bounds, next_pos)) continue;
        if (willBeOutOfMap(state.future_bounds, next_pos)) continue;
        
        int score = 0;
        
        // Check if this position avoids all bullets
        bool avoids_bullets = true;
        for (const Position& bullet : state.bullets) {
            // Calculate if bullet will hit this position
            int dx = abs(bullet.x - next_pos.x);
            int dy = abs(bullet.y - next_pos.y);
            
            // If bullet is on same row/column and within range
            if ((dx == 0 && dy <= BULLET_SPEED * 2) || 
                (dy == 0 && dx <= BULLET_SPEED * 2)) {
                avoids_bullets = false;
                score -= 50;  // Penalize positions in bullet path
            }
        }
        
        if (avoids_bullets) {
            score += 100;  // Big bonus for avoiding bullets
        }
        
        // Prefer staying near center
        score -= calculateDistance(next_pos, Position(state.center_x, state.center_y));
        
        // Prefer positions away from edges
        if (isNearMapEdge(state.current_bounds, next_pos)) {
            score -= 30;
        }
        
        if (score > best_score) {
            best_score = score;
            best_move = move;
        }
    }
    
    return best_move;
}

Move AIPlayer::findPositioningMove(const AIState& state) const {
    // Find a move that positions us for a better attack angle
    int dx = state.enemy_pos.x - state.my_pos.x;
    int dy = state.enemy_pos.y - state.my_pos.y;
    
    // Determine the best direction to face for attack
    Direction best_attack_dir;
    if (abs(dx) >= abs(dy)) {
        // Enemy is more horizontal, try to align horizontally
        best_attack_dir = (dx > 0) ? D_Right : D_Left;
    } else {
        // Enemy is more vertical, try to align vertically
        best_attack_dir = (dy > 0) ? D_Down : D_Up;
    }
    
    // If already facing the right direction, move forward to get closer
    if (state.my_dir == best_attack_dir) {
        Position forward_pos = getNextPosition(state.my_pos, state.my_dir, M_Forward);
        if (!willBeOutOfMap(state.current_bounds, forward_pos) &&
            !willBeInFutureDanger(state, forward_pos) &&
            isSafePosition(state, forward_pos)) {
            return M_Forward;
        }
    }
    
    // Otherwise, turn towards the best attack direction
    if (turnLeft(state.my_dir) == best_attack_dir) {
        return M_Left;
    } else if (turnRight(state.my_dir) == best_attack_dir) {
        return M_Right;
    }
    
    // If we need to turn 180 degrees, just turn left (arbitrary choice)
    if (getOppositeDirection(state.my_dir) == best_attack_dir) {
        return M_Left;
    }
    
    return M_Forward;
}
