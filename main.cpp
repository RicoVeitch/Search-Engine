#include<iostream>
#include "search.h"
#include "indexer.h"

void help(){
    std::cerr << "USAGE: ./search" << " <option> <file>\n"
        << "Options:\n"
        << "-i wsj.xml, -- to preform indexing on the data\n"
        << "-s query, -- to search for a query" << std::endl;
}

int main(int argc, char** argv) {
    if(argc <= 1){
        help();
        return EXIT_FAILURE;
    }
    std::string option = argv[1];
    if(option == "-i"){
        std::cout << "Index" << std::endl;
    }else if(option == "-s"){ //  && argc == 3
        std::string query;
        while(std::getline(std::cin, query)) {
            std::cout << "Seaching for: " << query << std::endl;
            SE::search(query);
        }
    }else{
        help();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}