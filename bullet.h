// bullet.h

#ifndef BULLET_H
#define BULLET_H

#include "common.h"

class Tank; 

class Bullet {
private:
    int x, y; // bullet position
    Direction direction; 
    bool active; // flying or hit/out
    char owner_id; // A or B

public:
    Bullet(int init_x, int init_y, Direction init_dir, char owner);
    ~Bullet();
    
    void move();
    void deactivate();
    
    bool checkCollisionWithTank(const Tank& tank) const;
    bool isAtPosition(int check_x, int check_y) const;
    
    int getX() const { return x; }
    int getY() const { return y; }
    Direction getDirection() const { return direction; }
    bool isActive() const { return active; }
    char getOwnerId() const { return owner_id; }
    
    void setPosition(int new_x, int new_y);
    void setActive(bool new_active);
    
    void getNextPosition(int& next_x, int& next_y) const;
    bool isOutOfBounds(int map_size) const;
};

#endif // BULLET_H