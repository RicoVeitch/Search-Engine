#!/bin/sh
cd src
g++ -std=c++11 search.cpp main.cpp indexer.cpp server_socket.cpp -o search_engine
./search_engine -i
cd ..
vagrant up