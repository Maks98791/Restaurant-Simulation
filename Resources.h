//
// Created by maks on 11.05.2020.
//

#ifndef SO2_RESTAURANT_RESOURCES_H
#define SO2_RESTAURANT_RESOURCES_H

#include <condition_variable>
#include <shared_mutex>
#include <atomic>

// mutexy i zmienne warunkowe
class Resources
{
public:
    // available place in table in dinning room
    static std::mutex mtxPlaceInTable;
    static std::condition_variable cvPlaceInTable;
    static std::atomic<bool> tableReady;

    // available place in dishwasher
    static std::mutex mtxPlaceInDishwasher;
    static std::condition_variable cvPlaceInDishwasher;
    static std::atomic<bool> dishwasherReady;

    // order taken from client by waiter
    static std::mutex mtxOrder;
    static std::condition_variable cvOrder;
    static std::atomic<bool> orderReady;

    // cooker needed to prepare food
    static std::mutex mtxCooker;
    static std::condition_variable cvCooker;
    static std::atomic<bool> cookerReady;

    static std::atomic<int> clientsInDinningRoom;

    static std::atomic<int> clientsInKitchen;

    static std::atomic<int> clientsInQueue;

    static std::atomic<int> platesInWashingRoom;

    static std::atomic<int> platesInKitchen;

    static std::mutex dinningClientmutex;

    static std::mutex platesMutex;
};


#endif //SO2_RESTAURANT_RESOURCES_H
