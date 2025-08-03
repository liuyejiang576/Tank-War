// game_map.h

#include "game_map.h"
#include "tank.h"

GameMap::GameMap(int init_size) 
    : current_size(init_size), turn_count(0), initial_size(init_size) {
}

GameMap::~GameMap() {}

void GameMap::updateTurn() {
    turn_count++;
    
    if (shouldShrink()) shrinkMap();
}

void GameMap::shrinkMap() {
    if (current_size > 2) current_size -= 2; // left-1, right-1
}

bool GameMap::shouldShrink() const {
    // once every 16 times
    return turn_count > 0 && turn_count % MAP_SHRINK_INTERVAL == 0;
}

bool GameMap::isInBounds(int x, int y) const {
    int center = getMapCenter();
    int half_size = current_size / 2;
    
    return x >= (center - half_size) && x < (center + half_size) &&
           y >= (center - half_size) && y < (center + half_size);
}

bool GameMap::isOnBorder(int x, int y) const {
    int center = getMapCenter();
    int half_size = current_size / 2;
    
    int min_x = center - half_size;
    int max_x = center + half_size - 1;
    int min_y = center - half_size;
    int max_y = center + half_size - 1;
    
    return ((x == min_x || x == max_x) && (y >= min_y && y <= max_y)) ||
           ((y == min_y || y == max_y) && (x >= min_x && x <= max_x));
}

bool GameMap::isOutOfBounds(int x, int y) const {
    return !isInBounds(x, y);
}

bool GameMap::shouldTakeDamageOutOfMap(const Tank& tank) const {
    return isOutOfBounds(tank.getX(), tank.getY());
}

void GameMap::setTurnCount(int count) {
    turn_count = count;
}

void GameMap::setCurrentSize(int size) {
    if (size > 0) {
        current_size = size;
    }
}

int GameMap::getMapCenter() const {
    return initial_size / 2;
}

int GameMap::getMinX() const {
    int center = getMapCenter();
    return center - current_size / 2;
}

int GameMap::getMaxX() const {
    int center = getMapCenter();
    return center + current_size / 2 - 1;
}

int GameMap::getMinY() const {
    int center = getMapCenter();
    return center - current_size / 2;
}

int GameMap::getMaxY() const {
    int center = getMapCenter();
    return center + current_size / 2 - 1;
}

void GameMap::reset() {
    current_size = initial_size;
    turn_count = 0;
}

bool GameMap::isMapShrinking() const {
    return current_size < initial_size;
}