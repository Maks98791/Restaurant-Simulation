#include <iostream>
#include <ncurses.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <random>
#include "Resources.h"
#include <string>
#include "Client.h"
#include "UtilityRoom.h"
#include <vector>
#include <atomic>

// ALL THREADS:
// unlimited clients, draw on the screen, end program, clean plates, prepare food(kucharz), wait in queue, get order(kelner)
// ALL RESOURCES:
// place in table(miejsce przy stole), place in dishwasher(miejsce w zmywarce), order(zamowienie), cooker(piekarnik),
// wspoldzielone - clients in room(liczba klientow w pokoju), clients in queue(liczba klientow w kolejce), plates in room(liczba talerzy w pokoju)

// sign symbolics a client
#define CLIENT_SIGN 'o'
// max amount of time to appear a new client
#define MAX_TIME 8
// min amount of time to appear a new client
#define MIN_TIME 5

// seed for random numbers
unsigned long seed = (unsigned long)(time(nullptr));
// random numbers generator
std::mt19937_64 rng(seed);
// random number between 2 and 3 seconds
std::uniform_int_distribution<int> randInt(MIN_TIME, MAX_TIME);

// vector of clients
std::vector <Client*> clients;
// vector of clients threads
std::vector <std::thread*> clientsThread;

// size of window
Position max_size;

// available rooms in restaurant
UtilityRoom *kitchen = new UtilityRoom(0,0,7,21, "Kitchen");
UtilityRoom *dinningRoom = new UtilityRoom(0,0,7,35, "Dinning Room");
UtilityRoom *washingRoom = new UtilityRoom(0,0,7,18, "Washing Room");
// hallway between other rooms
UtilityRoom *CrossingRoom;

// variable to quit program
bool quitProgram = false;
// variable to quit threads
bool quitThread = true;

// initializing function
void Initialize();
// displaying function
void Display();
// function checking if user wanted to exit
void Quit();
// function adding a new client
void AddNewClient();
// function removing exited clients
void RemoveExitedClients();
// function cleaning plates
void CleanPlates(UtilityRoom * washingRoom, UtilityRoom * kitchen);
// function giving a meal
void PrepareFood(UtilityRoom * kitchen);
// eating function
void Eat(UtilityRoom * dinningRoom);
// order a meal
void Order(UtilityRoom * kitchen);

void Initialize()
{
    initscr();
    noecho();
    curs_set(0);

    getmaxyx(stdscr, max_size.y, max_size.x);

    kitchen->x = max_size.x/2;
    dinningRoom->x = max_size.x/2;
    dinningRoom->y = max_size.y - dinningRoom->xSize;
    washingRoom->y = max_size.y - washingRoom->xSize;
    kitchen->setPlates(PLATES);

    int corh = max_size.y - kitchen->xSize - dinningRoom->xSize;
    int corw = max_size.x;

    CrossingRoom = new UtilityRoom(0,kitchen->xSize, corh, corw, "Queue:");
}

// displaying all resources and threads
void Display()
{
    while(!quitProgram)
    {
        WINDOW * kit = newwin(kitchen->xSize, kitchen->ySize, kitchen->y, kitchen->x - (kitchen->ySize/2));
        box(kit, 0, 0);
        touchwin(kit);
        mvwprintw(kit, 1, 1, kitchen->GetName());
        mvwprintw(kit, 2, 1, "In queue: %d", Resources::clientsInKitchen.load());
        mvwprintw(kit, 3, 1, "Plates: %d/%d", Resources::platesInKitchen.load(), PLATES);

        if(kitchen->ordering_status == true)
        {
            mvwprintw(kit, 4, 1, "Taking order!");
        }

        if(kitchen->cooking_status == true)
        {
            mvwprintw(kit, 5, 1, "Preparing meal!");
        }

        wrefresh(kit);

        WINDOW *din = newwin(dinningRoom->xSize, dinningRoom->ySize, dinningRoom->y,
                             dinningRoom->x - (dinningRoom->ySize / 2));
        box(din, 0, 0);
        touchwin(din);
        mvwprintw(din, 1, 1, dinningRoom->GetName());
        mvwprintw(din, 2, 1, "Eating: %d/%d", Resources::clientsInDinningRoom.load(), TABLES);
        mvwprintw(din, 3, 1, "All inside: %d", Resources::clientsInDinningRoom.load());
        wrefresh(din);

        WINDOW *dish = newwin(washingRoom->xSize, washingRoom->ySize, washingRoom->y, washingRoom->x);
        box(dish, 0, 0);
        touchwin(dish);
        mvwprintw(dish, 1, 1, washingRoom->GetName());
        mvwprintw(dish, 2, 1, "Plates: %d/%d", Resources::platesInWashingRoom.load(), PLATES);

        if(washingRoom->washing_status == true)
        {
            mvwprintw(dish, 4, 1, "Washing plate!");
        }
        else
        {
            mvwprintw(dish, 4, 1, "Recovering!");
        }

        wrefresh(dish);

        WINDOW *cor = newwin(CrossingRoom->xSize, CrossingRoom->ySize, CrossingRoom->y, CrossingRoom->x);
        touchwin(cor);
        mvwprintw(cor, 0, 33, CrossingRoom->GetName());
        mvwprintw(cor, 2, 1, "Clients inside: %d", clients.size());

        for (auto &client : clients)
        {
            mvwaddch(cor, client->pos.y, client->pos.x, CLIENT_SIGN);
        }

        wrefresh(cor);
        RemoveExitedClients();
        usleep(100000);
    }

    while (quitThread)
    {
        clear();
        mvprintw(max_size.y/2, max_size.x/2-7, "Closing .  ");
        refresh();
        usleep(500000);
        clear();
        mvprintw(max_size.y/2, max_size.x/2-7, "Closing .. ");
        refresh();
        usleep(500000);
        clear();
        mvprintw(max_size.y/2, max_size.x/2-7, "Closing ...");
        refresh();
        usleep(500000);
    }
}

// quiting program after clicking ESC
void Quit()
{
    while(!quitProgram)
    {
        if(getch() == 27)
        {
            quitProgram = true;
        }
    }
}

// adding new client and waiting random amount of time
void AddNewClient()
{
    Client *client = new Client(CrossingRoom->ySize - 2, 0, kitchen, dinningRoom, washingRoom);
    clients.push_back(client);
    std::thread *thread = new std::thread(Client::Begin, client);
    clientsThread.push_back(thread);

    sleep(randInt(rng));

}

// removing exited clients
void RemoveExitedClients()
{
    for(int i=0; i < clients.size(); i++)
    {
        if(clients.at(i)->isExit)
        {
            clientsThread.erase(clientsThread.begin() + i);
            clients.erase(clients.begin() + i);
        }
    }
}

// plates cleaning function -> run by Dishwasher thread
void CleanPlates(UtilityRoom *washingRoom, UtilityRoom *kitchen)
{
    while(!quitProgram)
    {
        if(Resources::platesInWashingRoom > 0)
        {
            // start cleaning
            washingRoom->washing_status = true;
            sleep(CLEAN_TIME);
            // end washing
            washingRoom->washing_status = false;

            // to jest chronione mutexem zeby w momencie zabrania talerza w zmywarce dodalo rownoczesnie talerz w kuchni
            // bo inaczej jakis inny watek moze sie wcisnac i namieszac
            Resources::platesMutex.lock();
            Resources::platesInWashingRoom--;
            Resources::platesInKitchen++;
            Resources::platesMutex.unlock();

            // unlocking dishwasher place for next plate
            Resources::dishwasherReady = true;
            Resources::cvPlaceInDishwasher.notify_one();
        }
        else
        {
            sleep(1);
        }
    }
}

// prepare food function -> run by CookChef thread
void PrepareFood(UtilityRoom *kitchen)
{
    while(!quitProgram)
    {
        Resources::platesMutex.lock();

        if(Resources::clientsInKitchen > 0 && Resources::platesInKitchen > 0)
        {
            Resources::platesInKitchen--;
            Resources::platesMutex.unlock();

            sleep(1);
            // start cooking
            kitchen->cooking_status = true;
            sleep(2);
            // end cooking
            kitchen->cooking_status = false;

            Resources::clientsInKitchen--;

            // unlocking place in cooker for next food
            Resources::cookerReady = true;
            Resources::cvCooker.notify_one();


            sleep(1);
            Resources::cookerReady = false;
        }
        else
        {
            Resources::platesMutex.unlock();
            sleep(1);
        }
    }
}

// eating function -> run by TableWaiter thread
void Eat(UtilityRoom *dinningRoom)
{
    while(!quitProgram)
    {
        Resources::dinningClientmutex.lock();

        if(Resources::clientsInQueue > 0 && !dinningRoom->isFull)
        {
            Resources::clientsInQueue++;
            Resources::clientsInDinningRoom++;

            if(Resources::clientsInDinningRoom >= TABLES)
            {
                dinningRoom->isFull = true;
            }

            Resources::dinningClientmutex.unlock();

            Resources::tableReady = true;
            Resources::cvPlaceInTable.notify_one();
        }
        else
        {
            Resources::dinningClientmutex.unlock();
            sleep(1);
        }
    }
}

// run by Waiter thread
void Order(UtilityRoom *kitchen)

{
    while(!quitProgram)
    {
        if(Resources::clientsInKitchen > 0)
        {
            // start taking order
            kitchen->ordering_status = true;
            sleep(1);
            // end taking order
            kitchen->ordering_status = false;

            // unlocking waiter thread
            Resources::orderReady = true;
            Resources::cvOrder.notify_one();
        }
        else
        {
            sleep(1);
        }
    }
}

int main()
{
    // Initializing variables for program start
    Initialize();

    // creating threads
    std::thread endingProgramThread(Quit);
    std::thread drawingOnScreenThread(Display);
    std::thread orderWaiterThread(Order, kitchen);
    std::thread cookChefThread(PrepareFood, kitchen);
    std::thread tablesWaiterThread(Eat, dinningRoom);
    std::thread platesWasherThread(CleanPlates, washingRoom, kitchen);

    while(!quitProgram)
    {
        AddNewClient();
    }

    // WHILE PRESS ESC
    endingProgramThread.join();
    platesWasherThread.join();
    cookChefThread.join();
    tablesWaiterThread.join();
    orderWaiterThread.join();

    for(auto &client : clients)
    {
        client->isExit = true;
    }

    Resources::cvCooker.notify_all();
    Resources::cvPlaceInTable.notify_all();
    Resources::cvPlaceInDishwasher.notify_all();
    Resources::cvOrder.notify_all();

    RemoveExitedClients();

    quitThread = false;
    drawingOnScreenThread.join();

    endwin();

    return 0;
}