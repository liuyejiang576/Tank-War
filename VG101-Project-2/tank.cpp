// tank.cpp

#include "tank.h"
#include <iostream>

Tank::Tank(int init_x, int init_y, Direction init_dir, int init_life, char id)
    : x(init_x), y(init_y), direction(init_dir), life_points(init_life), 
      shoot_counter(0), tank_id(id) {
}

Tank::~Tank() {}

void Tank::move(Move move_type) {
    switch (move_type) {
        case M_Forward: moveForward(); break;
        case M_Left: turnLeft(); break;
        case M_Right: turnRight(); break;
    }
}

void Tank::moveForward() {
    switch (direction) {
        case D_Left: x -= TANK_SPEED; break;
        case D_Up: y -= TANK_SPEED; break;
        case D_Right: x += TANK_SPEED; break;
        case D_Down: y += TANK_SPEED; break;
    }
}

void Tank::turnLeft() {
    direction = ::turnLeft(direction);
}

void Tank::turnRight() {
    direction = ::turnRight(direction);
}

bool Tank::canShoot() const {
    return shoot_counter == 0;
}

void Tank::updateShootCounter() {
    if (shoot_counter > 0) {
        shoot_counter--;
    }
}

void Tank::resetShootCounter() {
    shoot_counter = SHOOT_INTERVAL - 1; // cooling time
}

void Tank::takeDamage(int damage) {
    life_points -= damage;
    if (life_points < 0) {
        life_points = 0;
    }
}

bool Tank::isAlive() const {
    return life_points > 0;
}

void Tank::setPosition(int new_x, int new_y) {
    x = new_x;
    y = new_y;
}

void Tank::setDirection(Direction new_dir) {
    direction = new_dir;
}

void Tank::setLifePoints(int new_life) {
    life_points = new_life;
    if (life_points < 0) {
        life_points = 0;
    }
}

bool Tank::isAtPosition(int check_x, int check_y) const {
    return x == check_x && y == check_y;
}

void Tank::getNextPosition(int& next_x, int& next_y) const {
    next_x = x;
    next_y = y;
    
    switch (direction) {
        case D_Left: next_x -= TANK_SPEED; break;
        case D_Up: next_y -= TANK_SPEED; break;
        case D_Right: next_x += TANK_SPEED; break;
        case D_Down: next_y += TANK_SPEED; break;
    }
}

void Tank::getBulletSpawnPosition(int& bullet_x, int& bullet_y) const {
    bullet_x = x;
    bullet_y = y;
    
    // where the bullets are generated
    switch (direction) {
        case D_Left:
            bullet_x -= BULLET_SPAWN_DISTANCE;
            break;
        case D_Up:
            bullet_y -= BULLET_SPAWN_DISTANCE;
            break;
        case D_Right:
            bullet_x += BULLET_SPAWN_DISTANCE;
            break;
        case D_Down:
            bullet_y += BULLET_SPAWN_DISTANCE;
            break;
    }
}