#include <iostream>
#include "search.h"
#include "indexer.h"
#include "server_socket.h"

void help(){
    std::cerr << "USAGE: ./search" << " <option> <file>\n"
        << "Options and their files:\n"
        << "-i <path to wsj.xml file> -- to preform indexing on the wsj.xml file\n"
        << "-s <query test file> -- to search for a query\n";
}

int main(int argc, char** argv) {
    SE::Indexer indexer;
    SE::Search search;
    if(argc <= 1){
        help();
        return EXIT_FAILURE;
    }
    std::string option = argv[1];
    if(option == "-i"){
        if(argc != 3){
            help();
            return EXIT_FAILURE;
        }
        indexer.index(std::string(argv[2]));
    }else if(option == "-s"){ 
        // std::string raw_query;
        // while(std::getline(std::cin, raw_query)) {
        //     std::istringstream iss(raw_query);
        //     std::string query;
        //     std::string token;
        //     while(iss >> token){
            //     if(indexer.stop_words.find(token) == indexer.stop_words.end()){
            //         query += token + " ";
            //     }
            //     std::cout << token << "\n";
            // }
        //     query.pop_back();
        //     search.search(query);
        // }
        SE::Server_Socket server_socket;
        server_socket.set_up();
        char *request;
        std::string raw_query;
        std::cout << "new\n";
        while((request = server_socket.accept_client()) != NULL){
            raw_query = std::string(request);
            std::istringstream iss(raw_query);
            std::string query;
            std::string token;
            while(iss >> token){
                if(indexer.stop_words.find(token) == indexer.stop_words.end()){
                    query += token + " ";
                }
            }
            query.pop_back();
            std::string result = search.search(query);
            server_socket.send_client(result);
        }

    }else{
        help();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}