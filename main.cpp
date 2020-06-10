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
        char *token;
        std::string raw_query;
        while((token = server_socket.accept_client()) != NULL){
            raw_query = std::string(token);
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
            std::cout << result << "\n";
            server_socket.send_client(result);
        }

    }else{
        help();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}