#ifndef SEARCH_H 
#define SEARCH_H
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <math.h>
#include <cstdio>
#include <ctime>

namespace SE{
    const float k1 = 1.2;
    const int k3 = 7;
    const float b = 0.75;
    void load_block_info(std::vector<unsigned long> &res, std::ifstream &indexing_in);
    std::string walk_back(unsigned long mid, std::ifstream &indexing_in);
    bool bsearch_indexing(std::string &target_term, std::ifstream &indexing_in);
    float bm25(int dl, float adl, int N, long nt, int tf, int tq);
    void print_query_ranking(std::unordered_map<std::string, float> &query_results);
    void search(std::string query);
}

#endif