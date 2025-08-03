// bullet.cpp

#include "bullet.h"
#include "tank.h"

Bullet::Bullet(int init_x, int init_y, Direction init_dir, char owner)
    : x(init_x), y(init_y), direction(init_dir), active(true), owner_id(owner) {
}

Bullet::~Bullet() {}

void Bullet::move() {
    if (!active) return;
    
    for (int i = 0; i < BULLET_SPEED; i++) {
        switch (direction) {
            case D_Left: x--; break;
            case D_Up: y--; break;
            case D_Right: x++; break;
            case D_Down: y++; break;
        }
    }
}

void Bullet::deactivate() {
    active = false;
}

bool Bullet::checkCollisionWithTank(const Tank& tank) const {
    if (!active) return false;
    return isAtPosition(tank.getX(), tank.getY()) && 
           owner_id != tank.getTankId();
}

bool Bullet::isAtPosition(int check_x, int check_y) const {
    return x == check_x && y == check_y;
}

void Bullet::setPosition(int new_x, int new_y) {
    x = new_x;
    y = new_y;
}

void Bullet::setActive(bool new_active) {
    active = new_active;
}

void Bullet::getNextPosition(int& next_x, int& next_y) const {
    next_x = x;
    next_y = y;
    
    // move BULLET_SPEED paces to the next position
    for (int i = 0; i < BULLET_SPEED; i++) {
        switch (direction) {
            case D_Left: next_x--; break;
            case D_Up: next_y--; break;
            case D_Right: next_x++; break;
            case D_Down: next_y++; break;
        }
    }
}

bool Bullet::isOutOfBounds(int map_size) const {
    // use larger borders to simulate that the bullet can fly some distance out of the map
    const int boundary = map_size + BULLET_OUT_OF_BOUNDS_OFFSET;
    return x < -boundary || x >= boundary || y < -boundary || y >= boundary;
}