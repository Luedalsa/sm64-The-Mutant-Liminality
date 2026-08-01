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
    static std::vector<std::array<int, 5>> toads;
    static std::vector<std::array<int, 3>> oneUps;
    static std::vector<std::array<int, 3>> vanishCaps;
    static std::vector<std::array<int, 3>> wingCaps;
    static std::vector<std::array<int, 3>> metalCaps;
    static std::vector<std::array<int, 4>> warpPipes;
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

    static void spawnToad(int x, int y, int z, int yaw, int dialogId) {
        toads.push_back({x, y, z, yaw, dialogId});
    }

    static void spawnOneUp(int x, int y, int z) {
        oneUps.push_back({x, y, z});
    }

    static void spawnVanishCap(int x, int y, int z) {
        vanishCaps.push_back({x, y, z});
    }

    static void spawnWingCap(int x, int y, int z) {
        wingCaps.push_back({x, y, z});
    }

    static void spawnMetalCap(int x, int y, int z) {
        metalCaps.push_back({x, y, z});
    }

    static void spawnWarpPipe(int x, int y, int z, int yaw) {
        warpPipes.push_back({x, y, z, yaw});
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

    static const std::vector<std::array<int, 5>>& getToads() {
        return toads;
    }

    static const std::vector<std::array<int, 3>>& getOneUps() {
        return oneUps;
    }

    static const std::vector<std::array<int, 3>>& getVanishCaps() {
        return vanishCaps;
    }

    static const std::vector<std::array<int, 3>>& getWingCaps() {
        return wingCaps;
    }

    static const std::vector<std::array<int, 3>>& getMetalCaps() {
        return metalCaps;
    }

    static const std::vector<std::array<int, 4>>& getWarpPipes() {
        return warpPipes;
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_ACTORSPAWNERMANAGER_H