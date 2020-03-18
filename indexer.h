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
    inline void to_lower(std::string &s);
    void save_dict(std::map<std::string, std::unordered_map<std::string, int> > &dict, std::unordered_map<std::string, int> &doc_lengths);
    void save_info(std::unordered_map<std::string, int> &doc_lengths);
    void parse(std::map<std::string, std::unordered_map<std::string, int> > &dict, std::unordered_map<std::string, int> &doc_lengths);
    void index();
}

#endif