#include <iostream>
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <vector>
#include <string>

int main(){
    int server_socket, new_socket, bytes_read; 
    struct sockaddr_in address; 
    const int PORT = 10002;
    int opt = 1; 
    int addrlen = sizeof(address); 
    char buffer[1024]; 
       
    // Create socket
    if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == 0) { 
        std::cout << "cant make socket\n";
        return 1; 
    } 
       
    // Attach socket to the port 10002 
    if (setsockopt(server_socket, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) { 
        std::cout << "cant make socket\n";
        return 1; 
    } 
    address.sin_family = AF_INET; 
    address.sin_port = htons(PORT); 
    
    // provide address for the server socket to the private address of the backend vm.
    if(inet_pton(AF_INET, "192.168.2.10", &address.sin_addr) <= 0 ) { 
        std::cout << "Invalid address Address not supported\n"; 
        return 1; 
    } 

    // bind the server socket to the address provided previously
    if (bind(server_socket, (struct sockaddr *)&address, sizeof(address))<0) { 
        std::cout << "bind fail\n";
        return 1; 
    } 

    if (listen(server_socket, 3) < 0) { 
        std::cout << "listen fail\n";
        return 1; 
    } 
    if ((new_socket = accept(server_socket, (struct sockaddr *)&address, (socklen_t*)&addrlen)) < 0) { 
        std::cout << "accept fail\n";
        return 1; 
    } 
    bytes_read = read(new_socket, buffer, sizeof(buffer)); 
    buffer[bytes_read] = '\0';
    std::cout << buffer << "\n"; 
    // send(new_socket , hello , strlen(hello) , 0 ); 
    // std::cout << "sent\n";
    return 0; 
}