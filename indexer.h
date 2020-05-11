#ifndef INDEXER_H 
#define INDEXER_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <math.h>

namespace SE{
    /**
     * Converts a string to lower case.  
     * @param s: string to convert.
    */
    inline void to_lower(std::string &s){
        transform(s.begin(), s.end(), s.begin(), ::tolower); 
    }
    /**
     * Unpacks the dict which contains the neccessary information to build the indexing and posting files, and saves
     * the contents to disk
     * @param dict: the dictionary mapping words to the documents that they appear in along with the counts.
     * @param doc_lengths: maps documents to the length.
    */
    void save_dict(std::map<std::string, std::unordered_map<std::string, int> > &dict, std::unordered_map<std::string, int> &doc_lengths);
    /**
     * Gets information on how many documents there are and the average length and stores it on disk
     * @param doc_lengths: maps documents to the length.
    */
    void save_info(std::unordered_map<std::string, int> &doc_lengths);
    /**
     * Cleans a string of any non alphanumeric characters e.g. [100-yards] -> [100], [yards]
    */
    void clean_string(std::string &line, std::vector<std::string> &res);
    /**
     * Parses the target file storing the nesseary information in dict and doc_lengths.
     * @param dict: the dictionary mapping words to the documents that they appear in along with the counts.
     * @param doc_lengths: maps documents to the length.
    */
    void parse(std::map<std::string, std::unordered_map<std::string, int> > &dict, std::unordered_map<std::string, int> &doc_lengths);
    /**
     * Entry point.
    */
    void index();
}

#endif