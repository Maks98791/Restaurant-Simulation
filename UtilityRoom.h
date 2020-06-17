//
// Created by maks on 02.05.2020.
//

#ifndef SO2_RESTAURANT_UTILITYROOM_H
#define SO2_RESTAURANT_UTILITYROOM_H

// liczba dostepnych talerzy
#define PLATES 8
// czas potrzebny na umycie talerza
#define CLEAN_TIME 8

#include <string>
#include <shared_mutex>
#include <mutex>
#include <thread>
#include "Resources.h"

class UtilityRoom
{
public:
    // positions x and y of Room
    int x;
    int y;
    // size of Room
    int xSize;
    int ySize;

    // checking if room is full
    bool isFull;
    // room name
    char * roomName;

    // checking if kitchen taking an order
    bool ordering_status = false;
    // checking if dishwasher cleaning any plate
    bool washing_status = false;
    // checking if meal is being preparing
    bool cooking_status = false;

    UtilityRoom(int x, int y, int xSize, int ySize, char * roomName);

    char * GetName();
    void setPlates(int number);
};


#endif //SO2_RESTAURANT_UTILITYROOM_H
