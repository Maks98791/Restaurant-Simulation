//
// Created by maks on 26.04.2020.
//

#ifndef SO2_RESTAURANT_CLIENT_H
#define SO2_RESTAURANT_CLIENT_H

#include "UtilityRoom.h"
#include <shared_mutex>

#define EATING_TIME 6
#define TABLES 4
#define GET_ORDER_TIME 1
#define COOK_TIME 2
#define CLEAN_PLATE_TIME 2

// position of client on terminal
struct Position
{
    int x;
    int y;
};

class Client
{
public:
    // constructor
    Client(int x, int y, UtilityRoom *kitchen, UtilityRoom *dinningRoom, UtilityRoom *washingRoom);

    // begin of the client whole run
    static void Begin(Client* client);
    // changing current room to the next one
    static void NextRoom(Client *client);
    // taking meal
    static void PrepareMeal(Client *client);
    // eating meal
    static void EatMeal(Client *client);
    // wash client dish
    static void WashDish(Client *client);
    // get order from client
    static void GetOrder(Client *client);

    // utility rooms available in restaurant
    UtilityRoom *kitchen;
    UtilityRoom *dinningRoom;
    UtilityRoom *washingRoom;

    // next room to visit
    int currentTarget = 0;
    // checking if client quiting the restaurant
    bool isExit = false;
    // position of client
    Position pos = Position{0,0};
};


#endif //SO2_RESTAURANT_CLIENT_H
