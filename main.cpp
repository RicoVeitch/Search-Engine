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
        SE::index();
    }else if(option == "-s"){ 
        std::string raw_query;
        while(std::getline(std::cin, raw_query)) {
            std::string query = "";
            std::istringstream iss(raw_query);
            while (getline(iss, raw_query, ' ')){
                std::vector<std::string> clean_word;
                SE::clean_string(raw_query, clean_word);
                for(std::string word : clean_word){
                    if(SE::stop_words.find(word) == SE::stop_words.end())
                        query += word + " ";
                }
            }
            query.pop_back();
            std::cout << "Seaching for: " << query << std::endl;
            SE::search(query);
        }
    }else{
        help();
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}