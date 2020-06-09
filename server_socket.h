#pragma once
#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h>
#include <string.h>
#include <string>  
namespace SE{
    class Server_Socket{
        private:
            int server_socket, new_socket;
            struct sockaddr_in address; 
            int addrlen = sizeof(address);
            char message[1024];
        public:
            int set_up();
            char *accept_client();
            void send_client(std::string &result);
            ~Server_Socket();
    };
}