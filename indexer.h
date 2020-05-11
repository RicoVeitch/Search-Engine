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
#include <unordered_set>
#include <math.h>

namespace SE{
    const std::unordered_set<std::string> stop_words({"i", "me", "my", "myself", "we", "our", "ours", "ourselves", "you", "your", "yours", "yourself", 
    "yourselves", "he", "him", "his", "himself", "she", "her", "hers", "herself", "it", "its", "itself", "they", "them", "their", "theirs", 
    "themselves", "what", "which", "who", "whom", "this", "that", "these", "those", "am", "is", "are", "was", "were", "be", "been", "being", 
    "have", "has", "had", "having", "do", "does", "did", "doing", "a", "an", "the", "and", "but", "if", "or", "because", "as", "until", "while", 
    "of", "at", "by", "for", "with", "about", "against", "between", "into", "through", "during", "before", "after", "above", "below", "to", "from", 
    "up", "down", "in", "out", "on", "off", "over", "under", "again", "further", "then", "once", "here", "there", "when", "where", "why", "how", 
    "all", "any", "both", "each", "few", "more", "most", "other", "some", "such", "no", "nor", "not", "only", "own", "same", "so", "than", "too", 
    "very", "s", "t", "can", "will", "just", "don", "should", "now"});
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