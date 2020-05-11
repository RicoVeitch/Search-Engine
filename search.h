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
    /**
     * Fetches the block location in the posting list for a word and the size of that block.
     * @param res: vector to store the block location and size
     * @param indexing_in: the indexing file stream.
    */
    void load_block_info(std::vector<unsigned long> &res, std::ifstream &indexing_in);
    /**
     * walks back character by character in the indexing file till it reaches the '~' delimiter
     * that notes the start of a word.
     * @param mid: the seek position in the indexing file.
     * @param indexing_in: the inverted index file stream.
     * @return: the word.
    */
    std::string walk_back(unsigned long mid, std::ifstream &indexing_in);
    /**
     * Preforms binary search on the inverted index.
     * @param target_term: the term to search for
     * @param indexing_in: the inverted index file stream.
     * @return: true if the word exist in the inverted index.
    */
    bool bsearch_indexing(std::string &target_term, std::ifstream &indexing_in);
    /**
     * Calculates and return the bm25 value.
     * @param dl: document length.
     * @param adl: average document length.
     * @param N: number of documents.
     * @param nt: number of terms.
     * @param tf: term frequncy.
     * @param tq: term query frequncy.
    */
    float bm25(int dl, float adl, int N, long nt, int tf, int tq);
    /**
     * Prints the results of the search in decending order according the bm25 results.
     * @param query_results: maps docID to its score.
    */
    void print_query_ranking(std::unordered_map<std::string, float> &query_results);
    /**
     * Loads in the amount of documents and the average document length.
    */
    void load_info(int &doc_amt, int &avg_doc_len);
    /**
     * Entry point.
    */
    void search(std::string query);
}

#endif