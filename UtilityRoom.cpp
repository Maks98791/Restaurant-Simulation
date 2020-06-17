//
// Created by maks on 02.05.2020.
//

#include "UtilityRoom.h"

UtilityRoom::UtilityRoom(int x, int y, int xSize, int ySize, char * roomName)
{
    this->x = x;
    this->y = y;
    this->xSize = xSize;
    this->ySize = ySize;
    this->roomName = roomName;
}

char * UtilityRoom::GetName()
{
    return roomName;
}

void UtilityRoom::setPlates(int number)
{
    Resources::platesInKitchen = number;
}