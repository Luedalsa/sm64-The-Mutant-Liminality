//
// Created by Luis Alvarez on 03/07/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_ACTORSPAWNERMANAGER_H
#define SM64_THE_MUTANT_LIMINALITY_ACTORSPAWNERMANAGER_H
#include <vector>
#include <array>

class ActorSpawnerManager {
    static std::vector<std::array<int, 4>> doors;
    static std::vector<std::array<int, 3>> goombas;
    static std::vector<std::array<int, 3>> coins;
public:
    static void spawnSpecialDoor(int x, int y, int z, int yaw) {
        doors.push_back({x, y, z, yaw});
    }

    static void spawnGoomba(int x, int y, int z) {
        goombas.push_back({x, y, z});
    }

    static void spawnCoin(int x, int y, int z) {
        coins.push_back({x, y, z});
    }

    static const std::vector<std::array<int, 4>>& getSpecialDoors() {
        return doors;
    }

    static const std::vector<std::array<int, 3>>& getGoombas() {
        return goombas;
    }

    static const std::vector<std::array<int, 3>>& getCoins() {
        return coins;
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_ACTORSPAWNERMANAGER_H
