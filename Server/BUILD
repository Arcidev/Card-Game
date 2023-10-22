#!/bin/bash
g++ -std=c++2a $@ -Wall -Wextra -o output/server Server/PacketHandlers/*.cpp Server/Cards/*.cpp Server/Spells/*.cpp Server/*.cpp Crypto/*.cpp Database/*.cpp Settings/*.cpp Server/Achievements/*.cpp -lssl -lcrypto -lpq -lpthread
