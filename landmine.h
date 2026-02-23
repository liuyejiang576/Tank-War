// landmine.h

#ifndef LANDMINE_H
#define LANDMINE_H

#include "common.h"
#include <vector>
#include <memory>

// Forward declaration
struct Position;


class Landmine {
private:
    int x, y;           // position
    bool active;        // is the landmine still active?
    bool visible;       // is the landmine visible to players?
    char placed_by;     // which tank placed it ('A' or 'B')
    int damage;         // damage dealt when triggered
    
public:
    Landmine(int pos_x, int pos_y, char owner, int dmg = 3);
    ~Landmine();
    
    // Getters
    int getX() const { return x; }
    int getY() const { return y; }
    bool isActive() const { return active; }
    bool isVisible() const { return visible; }
    char getOwner() const { return placed_by; }
    int getDamage() const { return damage; }
    
    // State management
    void activate() { active = true; }
    void detonate() { active = false; }
    void reveal() { visible = true; }
    void hide() { visible = false; }
    
    // Collision detection
    bool isAtPosition(int check_x, int check_y) const;
    bool checkTrigger(int tank_x, int tank_y) const;
};

// LandmineManager handles all landmines on the map
class LandmineManager {
private:
    std::vector<std::unique_ptr<Landmine>> landmines;
    int max_landmines;          // maximum landmines allowed on map
    int spawn_interval;         // turns between landmine spawns
    int turns_since_last_spawn;
    
public:
    LandmineManager(int max_mines = 5, int interval = 10);
    ~LandmineManager();
    
    // Landmine management
    void addLandmine(int x, int y, char owner, int damage = 3);
    void removeLandmine(int x, int y);
    void clearAllLandmines();
    
    // Game loop integration
    void update(int turn_count, int map_size);
    bool checkTankCollision(int tank_x, int tank_y, char tank_id, int& damage_out);
    
    // Getters
    const std::vector<std::unique_ptr<Landmine>>& getLandmines() const { return landmines; }
    int getLandmineCount() const { return landmines.size(); }
    bool hasLandmineAt(int x, int y) const;
    const Landmine* getLandmineAt(int x, int y) const;
    
    // Utility
    std::vector<Position> getLandminePositions() const;
};

#endif // LANDMINE_H