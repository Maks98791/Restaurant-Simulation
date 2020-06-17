//
// Created by maks on 26.04.2020.
//

#include <unistd.h>
#include <mutex>
#include "Client.h"
#include "Resources.h"
#include <shared_mutex>


Client::Client(int x, int y, UtilityRoom *kitchen, UtilityRoom *dinningRoom, UtilityRoom *washingRoom)
{
    this->pos.x = x;
    this->pos.y = y;
    this->dinningRoom = dinningRoom;
    this->washingRoom = washingRoom;
    this->kitchen = kitchen;
}

// running by client thread
void Client::Begin(Client* client)
{
    while(!client->isExit)
    {
        if((client->pos.x > 40 && client->pos.y == 0) || (client->pos.x >= 7 && client->pos.y >= 9))
        {
            client->pos.x--;
        }
        else if(client->pos.x <= 40 && client->pos.y < 9)
        {
            client->pos.y++;
        }
        else
        {
            client->isExit = true;
        }

        usleep(100000);
        NextRoom(client);
    }
}

void Client::NextRoom(Client *client)
{
    if(!client->isExit)
    {
        // checking what is our current room, and switch it properly
        if(client->currentTarget != 1 && client->pos.x <= 40 && client->pos.y < 9)
        {
            Client::GetOrder(client);
            Client::PrepareMeal(client);
            client->currentTarget = 1;
        }
        else if(client->currentTarget != 2 && client->pos.x > 7 && client->pos.y >= 9)
        {
            Client::EatMeal(client);
            client->currentTarget = 2;
        }
        else if(client->currentTarget != 3 && client->pos.x == 7 && client->pos.y == 9)
        {
            Client::WashDish(client);
            client->currentTarget = 3;
        }
    }
}

void Client::GetOrder(Client *client)
{
    if(!client->isExit)
    {
        Resources::clientsInKitchen++;

        // locking client thread and waiting for Waiter thread to run GetOrder() function
        std::unique_lock<std::mutex> lck(Resources::mtxOrder);
        while ( ![]{ return Resources::orderReady.load(); }())
        {
            Resources::cvOrder.wait(lck);
        }
    }
}

void Client::PrepareMeal(Client *client)
{
    if(!client->isExit)
    {
        // cook has to prepare meal so locking Cook thread by locking Cooker resource
        std::unique_lock<std::mutex> lck(Resources::mtxCooker);
        while ( ![]{ return Resources::cookerReady.load(); }())
        {
            Resources::cvCooker.wait(lck);
        }
    }
}

void Client::EatMeal(Client *client)
{
    if(!client->isExit)
    {
        if(!client->dinningRoom->isFull)
        {
            Resources::dinningClientmutex.lock();
            Resources::clientsInDinningRoom++;

            if(Resources::clientsInDinningRoom >= TABLES)
            {
                Resources::dinningClientmutex.unlock();

                client->dinningRoom->isFull = true;
                sleep(EATING_TIME);

                Resources::dinningClientmutex.lock();
                Resources::clientsInDinningRoom--;

                if(Resources::clientsInDinningRoom < TABLES)
                {
                    client->dinningRoom->isFull = false;
                    Resources::dinningClientmutex.unlock();
                }
                else
                {
                    Resources::clientsInQueue++;
                    Resources::dinningClientmutex.unlock();

                    // block the Eating thread by locking PlaceInTable resource
                    std::unique_lock<std::mutex> lck(Resources::mtxPlaceInTable);
                    while ( ![]{ return Resources::orderReady.load(); }())
                    {
                        Resources::cvPlaceInTable.wait(lck);
                    }

                    sleep(EATING_TIME);

                    Resources::dinningClientmutex.lock();
                    Resources::clientsInDinningRoom--;

                    if(Resources::clientsInDinningRoom < TABLES)
                    {
                        client->dinningRoom->isFull = false;
                        Resources::dinningClientmutex.unlock();
                    }
                }
            }
            else
            {
                Resources::dinningClientmutex.unlock();
            }
        }
    }
}

void Client::WashDish(Client *client)
{
    if(!client->isExit)
    {
        // increasing number of plates in room
        Resources::platesInWashingRoom++;

        // block the client thread and waiting for dishwasher thread to run CleanPlates() function in main
        std::unique_lock<std::mutex> lck(Resources::mtxPlaceInDishwasher);
        while ( ![]{ return Resources::dishwasherReady.load(); }())
        {
            Resources::cvPlaceInDishwasher.wait(lck);
        }
    }
}
