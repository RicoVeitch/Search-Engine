#pragma once

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <unordered_set>
#include <math.h>
#include <string.h> 
#include <cstring>

namespace SE{
    class Indexer{
        private:
            char token_buffer[1024 * 1024];
            char buffer[1024 * 1024];
            char *curr;
            const static uint8_t POST_ENTRY_SIZE;
            std::map<std::string, std::unordered_map<uint64_t, unsigned short> > dict; // word: doc_id: amount
            std::unordered_map<uint64_t, uint32_t> doc_lengths; // doc_id: doc_length
            struct Doc_Range{
                uint64_t doc_start, doc_end, title_start, title_end;
            };
            std::map<uint64_t, Doc_Range> doc_ranges;
        public:
            const static std::unordered_set<std::string> stop_words;  // taken from https://gist.github.com/sebleier/554280
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
            void save_dict();
            /**
             * Gets information on how many documents there are and the average length and stores it on disk
             * @param doc_lengths: maps documents to the length.
            */
            void save_info();
            /**
             * Saves the header and full range for each document in the collection. 
            */
            void save_doc_range();
            /**
             * Cleans a string of any non alphanumeric characters
             * @return char*, a pointer to the cleaned word.
            */
            char *clean_token();
            /**
             * Parses the target file storing the nesseary information in dict and doc_lengths.
             * @param dict: the dictionary mapping words to the documents that they appear in along with the counts.
             * @param doc_lengths: maps documents to the length.
            */
            void parse(std::string wsj_path);
            /**
             * Entry point.
            */
            void index(std::string wsj_path);
    };
}
