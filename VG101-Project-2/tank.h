// tank.h

#ifndef TANK_H
#define TANK_H

#include "common.h"

class Tank {
private:
    int x, y; // tank position
    Direction direction;         
    int life_points;            
    int shoot_counter; // cooling time      
    char tank_id; // A or B

public:
    Tank(int init_x, int init_y, Direction init_dir, int init_life, char id);
    ~Tank();
    
    void move(Move move_type);
    void moveForward();
    void turnLeft();
    void turnRight();
    
    bool canShoot() const;
    void updateShootCounter();
    void resetShootCounter();
    
    void takeDamage(int damage);
    bool isAlive() const;
    
    int getX() const { return x; }
    int getY() const { return y; }
    Direction getDirection() const { return direction; }
    int getLifePoints() const { return life_points; }
    int getShootCounter() const { return shoot_counter; }
    char getTankId() const { return tank_id; }
    
    void setPosition(int new_x, int new_y);
    void setDirection(Direction new_dir);
    void setLifePoints(int new_life);
    
    bool isAtPosition(int check_x, int check_y) const;
    void getNextPosition(int& next_x, int& next_y) const;
    void getBulletSpawnPosition(int& bullet_x, int& bullet_y) const;
};

#endif // TANK_H