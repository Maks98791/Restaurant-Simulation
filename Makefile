all:
	g++ -std=c++17 -o proj main.cpp Client.cpp Resources.cpp UtilityRoom.cpp Client.h Resources.h UtilityRoom.h -lncurses -lpthread
