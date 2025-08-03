// common.h

#ifndef COMMON_H
#define COMMON_H

enum Direction {
    D_Left = 0, D_Up = 1, D_Right = 2, D_Down = 3    
};

enum Move {
    M_Forward = 0, M_Left = 1, M_Right = 2     
};

enum GameMode {
    PVP, PVE, DEMO   
};

enum GameResult {
    GAME_CONTINUE, TANK_A_WIN, TANK_B_WIN, DRAW            
};

const int INITIAL_MAP_SIZE = 20;
const int BULLET_OUT_OF_BOUNDS_OFFSET = 10;
const int DEFAULT_LIFE_POINTS = 5;
const int BULLET_DAMAGE = 2;
const int BULLET_SPEED = 2;
const int TANK_SPEED = 1;
const int SHOOT_INTERVAL = 3;
const int MAP_SHRINK_INTERVAL = 16;
const int OUT_OF_MAP_DAMAGE = 1;
const int BULLET_SPAWN_DISTANCE = 2;

Direction turnLeft(Direction dir);
Direction turnRight(Direction dir);
Direction getOppositeDirection(Direction dir);

#endif // COMMON_H