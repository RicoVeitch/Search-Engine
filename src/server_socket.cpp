#include "server_socket.h"

namespace SE{
    
    int Server_Socket::set_up(){ 
        const int PORT = 10002;
        const char *ip_address = "192.168.2.10";

        int opt = 1; 
        // Create socket
        if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
            std::cerr << "cant make socket\n";
            return 0; 
        } 
        
        // Attach socket to the port 10002 
        if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
            std::cerr << "cant make socket\n";
            return 0; 
        } 
        address.sin_family = AF_INET; 
        address.sin_port = htons(PORT); 
        
        // provide address for the server socket to the private address of the backend vm.
        if(inet_pton(AF_INET, ip_address, &address.sin_addr) <= 0 ) { 
            std::cerr << "Invalid address\n"; 
            return 0; 
        } 

        // bind the server socket to the address provided previously
        if (bind(server_socket, (struct sockaddr *)&address, sizeof(address))<0) { 
            std::cerr << "bind failed\n";
            return 0; 
        } 

        // start listing.
        if (listen(server_socket, 3) < 0) { 
            std::cerr << "listen failed\n";
            return 0; 
        }
        return 1;
    }
    char *Server_Socket::accept_client() {
        int bytes_read;
        char buffer[1024]; 
        if ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) { 
            std::cout << "accept fail\n";
            return NULL; 
        } 
        bytes_read = read(new_socket, buffer, sizeof(buffer)); 
        memcpy(message, buffer, bytes_read);
        message[bytes_read] = '\0';
        return message; 
    }

    void Server_Socket::send_client(std::string &result){
        send(new_socket, result.data(), result.length(), 0);
        close(new_socket);
    }

    Server_Socket::~Server_Socket(){
        close(server_socket);
    }
};