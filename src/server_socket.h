#pragma once
#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h>
#include <string.h>
#include <string>  
#include <cstring>
namespace SE{
    class Server_Socket{
        private:
            int server_socket, new_socket;
            struct sockaddr_in address; 
            int addrlen = sizeof(address);
            char message[1024];
        public:
            /**
             * Sets up the server socket
             * @return 0 if failed else 1.
             */
            int set_up();
            /**
             * Accepts a client connection with their message  
             * @return char pointer to their message.
             */
            char *accept_client();
            /**
             * Send the same accepted client a message back.
             * Either the documents retrived along with their headers
             * or the document itself that they requested.
             * @param result: the message to be sent back.
             */ 
            void send_client(std::string &result);
            ~Server_Socket();
    };
}