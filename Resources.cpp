//
// Created by maks on 11.05.2020.
//

#include "Resources.h"

std::mutex Resources::mtxPlaceInTable;
std::condition_variable Resources::cvPlaceInTable;
std::atomic<bool> Resources::tableReady{false};

std::mutex Resources::mtxPlaceInDishwasher;
std::condition_variable Resources::cvPlaceInDishwasher;
std::atomic<bool> Resources::dishwasherReady{false};

std::mutex Resources::mtxOrder;
std::condition_variable Resources::cvOrder;
std::atomic<bool> Resources::orderReady{false};

std::mutex Resources::mtxCooker;
std::condition_variable Resources::cvCooker;
std::atomic<bool> Resources::cookerReady{false};

std::atomic<int> Resources::clientsInDinningRoom(0);

std::atomic<int> Resources::clientsInQueue(0);

std::atomic<int> Resources::platesInWashingRoom(0);

std::atomic<int> Resources::clientsInKitchen(0);

std::atomic<int> Resources::platesInKitchen(0);

std::mutex Resources::dinningClientmutex;

std::mutex Resources::platesMutex;
