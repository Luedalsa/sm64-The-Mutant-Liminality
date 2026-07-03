//
// Created by Luis Alvarez on 03/07/2026.
//

#ifndef SM64_THE_MUTANT_LIMINALITY_LEVELPOOLMANAGER_H
#define SM64_THE_MUTANT_LIMINALITY_LEVELPOOLMANAGER_H
#include <cstdlib>
#include <vector>

class LevelPoolManager {
    static std::vector<void *> poolPointers;
public:

    static void freeLevelPool() {
        for (void* ptr : poolPointers) {
            free(ptr);
        }
        poolPointers.clear();
    }

    template <typename T>
    static T* allocOnPool(size_t count = 1) {
        T* ptr = static_cast<T*>(malloc(sizeof(T) * count));
        poolPointers.push_back(ptr);
        return ptr;
    }
};

#endif // SM64_THE_MUTANT_LIMINALITY_LEVELPOOLMANAGER_H
