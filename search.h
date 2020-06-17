#pragma once
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
    class Search{
        private:
            const float k1 = 1.2;
            const int k3 = 7;
            const float b = 0.75;
            const uint8_t INDEX_ENTRY_SIZE = 76;
        public:
            /**
             * Preforms binary search on the inverted index.
             * @param target_term: the term to search for
             * @param indexing_in: the inverted index file stream.
             * @return: true if the word exist in the inverted index.
            */
            bool bsearch_indexing(std::string target_term, std::ifstream &indexing_in);
            std::pair<uint64_t, uint64_t> bsearch_range(uint64_t target_doc, std::ifstream &range_in, bool header);
            std::string get_doc_content(uint64_t doc_id, bool header);
            /**
             * Calculates and return the bm25 value.
             * @param dl: document length.
             * @param adl: average document length.
             * @param N: number of documents.
             * @param nt: number of terms.
             * @param tf: term frequncy.
             * @param tq: term query frequncy.
            */
            double bm25(uint32_t dl, float adl, int N, uint32_t nt, uint16_t tf, int tq);
            /**
             * Prints the results of the search in decending order according the bm25 results.
             * @param query_results: maps docID to its score.
            */
            std::vector<uint64_t> print_query_ranking(std::unordered_map<uint64_t, double> &query_results);
            /**
             * Loads in the amount of documents and the average document length.
            */
            void load_info(int &doc_amt, int &avg_doc_len);
            /**
             * Entry point.
            */
            std::vector<uint64_t> search(std::string query);
    };
}
