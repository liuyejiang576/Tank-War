// game_map.h

#ifndef GAME_MAP_H
#define GAME_MAP_H

#include "common.h"

class Tank;

class GameMap {
private:
    int current_size;           
    int turn_count;             
    int initial_size;           

public:
    GameMap(int init_size = INITIAL_MAP_SIZE);
    ~GameMap();
    
    void updateTurn();
    void shrinkMap();
    bool shouldShrink() const;
    
    bool isInBounds(int x, int y) const;
    bool isOnBorder(int x, int y) const;
    bool isOutOfBounds(int x, int y) const;
    
    bool shouldTakeDamageOutOfMap(const Tank& tank) const;
    
    int getCurrentSize() const { return current_size; }
    int getTurnCount() const { return turn_count; }
    int getInitialSize() const { return initial_size; }

    void setTurnCount(int count);
    void setCurrentSize(int size);
    
    int getMapCenter() const;
    int getMinX() const;
    int getMaxX() const;
    int getMinY() const;
    int getMaxY() const;

    void reset();
    bool isMapShrinking() const;
};

#endif // GAME_MAP_H