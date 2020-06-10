#include "search.h"
#include "shared_const.h"

#define AMT 0
#define BLOCK 1

namespace SE{

    bool Search::bsearch_indexing(std::string target_term, std::ifstream &indexing_in){
        target_term.resize(64);
        uint64_t left = 0, right = 0, mid = 0;
        int n = (int) target_term.length();
        indexing_in.seekg(0, indexing_in.beg);
        left = indexing_in.tellg();
        indexing_in.seekg(0, indexing_in.end);
        right = indexing_in.tellg();

        while(left <= right){
            bool found = true;
            std::string word;
            word.resize(MAX_WORD_LEN); // MAX_WORD_LEN
            mid = left + (right - left) / 2;
            mid -= (mid % INDEX_ENTRY_SIZE); // round down to nearest 76th, 
            indexing_in.seekg(mid);
            indexing_in.read(&word[0], 64);
            //std::cout << word << "\n";
            if(word > target_term){
                right = mid - INDEX_ENTRY_SIZE;
            } else if(word < target_term){
                left = mid + INDEX_ENTRY_SIZE;
            } else{
                return true;
            }
        }
        return false;
    }

    double Search::bm25(uint32_t dl, float adl, int N, uint32_t nt, uint16_t tf, int tq){
        double K = k1 * ((1-b) + ((b * dl) / adl));
        return log2((N - nt + 0.5) / (nt + 0.5)) * ((k1 + 1) * tf) / (K + tf) * ((k3 + 1) * tq) / (k3 + tq);
    }

    std::string Search::print_query_ranking(std::unordered_map<uint64_t, double> &query_results){
        int max_docs = 10;
        std::string output = "";
        std::vector<std::pair<uint64_t, double> > res;
        std::copy(query_results.begin(), query_results.end(), std::back_inserter<std::vector<std::pair<uint64_t, double>>>(res));
        sort(res.begin(), res.end(),
                [](const std::pair<uint64_t, double>& l, std::pair<uint64_t, double>& r) {
                    if (l.second != r.second)
                        return l.second > r.second;

                    return l.first < r.first;
                });
        std::string filename;
        // for(int doc = 0; doc < (int) res.size(); doc++){
        //     filename = std::to_string(res[doc].first);
        //     filename.insert(6, 1, '-');
        //     std::cout << "WSJ" << filename << " "<< res[doc].second << "\n"; // res[doc].first
        // }
        for(int doc = 0; doc < 10; doc++){
            filename = std::to_string(res[doc].first);
            filename.insert(6, 1, '-');
            //std::cout << "WSJ" << filename << " "<< res[doc].second << "\n";
            output += "WSJ" + filename + "\n";
        }
        return output;
    }

    void Search::load_info(int &doc_amt, int &avg_doc_len){
        std::ifstream info_in;
        std::string line;
        info_in.open(info_name);
        getline(info_in, line);
        doc_amt = stoi(line);
        getline(info_in, line);
        avg_doc_len = stoi(line);
        info_in.close();
    }

    std::string Search::search(std::string query){
        std::string query_copy = query;

        // indexing information
        std::ifstream indexing_in;
        uint32_t term_doc_amt;
        uint64_t block_loc;        
        
        // varaibles to store amount of documents that were indexed, and average document length. 
        int doc_amt;
        int avg_doc_len;
        load_info(doc_amt, avg_doc_len);

        // Details for posting list
        FILE * posting_in;
        posting_in = fopen (posting_name.c_str() , "rb");
        uint64_t doc_id;
        uint16_t term_count;
        uint32_t doc_len;

        // Data structures to hold info, score for each docid and counts for each word in query.
        std::unordered_map<uint64_t, double> query_results; 
        std::unordered_map<std::string, int> query_term_count;

        std::istringstream qss(query_copy);
        while (getline(qss, query_copy, ' ')){
            query_term_count[query_copy]++;
        }
        
        std::istringstream iss(query);
        indexing_in.open(indexing_name, std::ofstream::binary);

        while (std::getline(iss, query, ' ')){
            if(bsearch_indexing(query, indexing_in)){
                indexing_in.read((char*)&term_doc_amt, sizeof(term_doc_amt));
                indexing_in.read((char*)&block_loc, sizeof(block_loc));
                fseek(posting_in, block_loc, SEEK_SET);
                int docs_read = 0;
                do{
                    fread(&doc_id, sizeof(doc_id), 1, posting_in);
                    fread(&term_count, sizeof(term_count), 1, posting_in);
                    fread(&doc_len, sizeof(doc_len), 1, posting_in);
                    query_results[doc_id] += bm25(doc_len, avg_doc_len, doc_amt, term_doc_amt, term_count, query_term_count[query]);
                    docs_read++;
                }while(docs_read < term_doc_amt);
            }
        }
        indexing_in.close();
        fclose(posting_in);
        if(query_results.empty()){
            return "No results found.\n";
        }
        return print_query_ranking(query_results);
    }
}