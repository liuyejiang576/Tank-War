// landmine.cpp

#include "landmine.h"
#include "ai_player.h"  // For Position struct
#include <algorithm>
#include <random>

// Landmine class implementation
Landmine::Landmine(int pos_x, int pos_y, char owner, int dmg) 
    : x(pos_x), y(pos_y), active(true), visible(false), placed_by(owner), damage(dmg) {
}

Landmine::~Landmine() {}

bool Landmine::isAtPosition(int check_x, int check_y) const {
    return active && x == check_x && y == check_y;
}

bool Landmine::checkTrigger(int tank_x, int tank_y) const {
    return active && x == tank_x && y == tank_y;
}

// LandmineManager class implementation
LandmineManager::LandmineManager(int max_mines, int interval) 
    : max_landmines(max_mines), spawn_interval(interval), turns_since_last_spawn(0) {
}

LandmineManager::~LandmineManager() {
    landmines.clear();
}

void LandmineManager::addLandmine(int x, int y, char owner, int damage) {
    if (landmines.size() < static_cast<size_t>(max_landmines)) {
        landmines.push_back(std::make_unique<Landmine>(x, y, owner, damage));
    }
}

void LandmineManager::removeLandmine(int x, int y) {
    landmines.erase(
        std::remove_if(landmines.begin(), landmines.end(),
            [x, y](const std::unique_ptr<Landmine>& mine) {
                return mine->isAtPosition(x, y);
            }),
        landmines.end()
    );
}

void LandmineManager::clearAllLandmines() {
    landmines.clear();
}

void LandmineManager::update(int turn_count, int map_size) {
    // Remove inactive landmines
    landmines.erase(
        std::remove_if(landmines.begin(), landmines.end(),
            [](const std::unique_ptr<Landmine>& mine) {
                return !mine->isActive();
            }),
        landmines.end()
    );
    
    // Possibly spawn a new landmine
    turns_since_last_spawn++;
    if (turns_since_last_spawn >= spawn_interval && 
        landmines.size() < static_cast<size_t>(max_landmines)) {
        
        static std::random_device rd;
        static std::mt19937 gen(rd());
        std::uniform_int_distribution<> pos_dis(2, map_size - 3);
        std::uniform_int_distribution<> owner_dis(0, 1);
        
        int new_x = pos_dis(gen);
        int new_y = pos_dis(gen);
        char owner = (owner_dis(gen) == 0) ? 'A' : 'B';
        
        // Don't spawn on existing landmines
        if (!hasLandmineAt(new_x, new_y)) {
            addLandmine(new_x, owner, 3);
            turns_since_last_spawn = 0;
        }
    }
}

bool LandmineManager::checkTankCollision(int tank_x, int tank_y, char tank_id, int& damage_out) {
    for (auto& mine : landmines) {
        if (mine->isActive() && mine->checkTrigger(tank_x, tank_y)) {
            // Landmine triggers if tank steps on it
            // But not if it's the owner's tank (owner is immune to their own mines)
            if (mine->getOwner() != tank_id) {
                damage_out = mine->getDamage();
                mine->detonate();
                return true;
            }
        }
    }
    return false;
}

bool LandmineManager::hasLandmineAt(int x, int y) const {
    for (const auto& mine : landmines) {
        if (mine->isActive() && mine->getX() == x && mine->getY() == y) {
            return true;
        }
    }
    return false;
}

const Landmine* LandmineManager::getLandmineAt(int x, int y) const {
    for (const auto& mine : landmines) {
        if (mine->isActive() && mine->getX() == x && mine->getY() == y) {
            return mine.get();
        }
    }
    return nullptr;
}

std::vector<Position> LandmineManager::getLandminePositions() const {
    std::vector<Position> positions;
    for (const auto& mine : landmines) {
        if (mine->isActive()) {
            positions.emplace_back(mine->getX(), mine->getY());
        }
    }
    return positions;
}