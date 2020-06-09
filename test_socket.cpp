#include <iostream>
#include "server_socket.h"

int main(){
    SE::Server_Socket server_socket;
    std::cout << "setting up\n";
    server_socket.set_up();
    char *token;
    while((token = server_socket.accept_client()) != NULL){
        std::cout << token << "\n";
        std::string res("send back");
        std::cout << "sending: " << res << "\n";
        server_socket.send_client(res);
    }
    return 0; 
}