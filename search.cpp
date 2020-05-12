#include "search.h"
#define AMT 0
#define BLOCK 1

namespace SE{

    void load_block_info(std::vector<unsigned long> &res, std::ifstream &indexing_in){
        std::string line;
        unsigned long temp;
        getline(indexing_in, line);
        std::istringstream iss(line);
        while (getline(iss, line, ' ')){
            if(line == "" || line == " ")
                continue;
            temp = stoul(line, nullptr, 0);
            res.push_back(temp);
        }
    } 

    std::string walk_back(unsigned long mid, std::ifstream &indexing_in){
        std::string word;
        indexing_in >> word;
        while(word[0] != '~'){
            mid--;
            indexing_in.seekg(mid);
            indexing_in >> word;
        }
        return word.substr(1, word.size());
    }

    bool bsearch_indexing(std::string &target_term, std::ifstream &indexing_in){
        unsigned long left = 0, right = 0, mid = 0;
        std::string word;
        bool found = false;

        if(!indexing_in.is_open()){
            std::cerr << "File closed" << std::endl;
        }
        indexing_in.seekg(0, indexing_in.beg);
        left = indexing_in.tellg();
        indexing_in.seekg(0, indexing_in.end);
        right = indexing_in.tellg();

        while(left <= right){
            mid = left + (right - left) / 2;
            indexing_in.seekg(mid);
            word = walk_back(mid, indexing_in);
            if(word > target_term){
                right = mid - 1;
            }else if(word < target_term){
                left = mid + 1;
            }else{
                found = true;
                break;
            }
        }
        return found;
    }

    float bm25(uint32_t dl, float adl, int N, long nt, uint16_t tf, int tq){
        float K = k1 * ((1-b) + ((b * dl) / adl));
        return log2((N - nt + 0.5) / (nt + 0.5)) * ((k1 + 1) * tf) / (K + tf) * ((k3 + 1) * tq) / (k3 + tq);
    }

    void print_query_ranking(std::unordered_map<uint64_t, float> &query_results){
        int max_docs = 10;
        std::vector<std::pair<uint64_t, float> > res;
        std::copy(query_results.begin(), query_results.end(), std::back_inserter<std::vector<std::pair<uint64_t, float>>>(res));
        sort(res.begin(), res.end(),
                [](const std::pair<uint64_t, float>& l, std::pair<uint64_t, float>& r) {
                    if (l.second != r.second)
                        return l.second > r.second;

                    return l.first < r.first;
                });
        for(int doc = 0; doc < max_docs && doc < res.size(); doc++){
            std::cout << doc+1 << ": WSJ" << res[doc].first << "\tscore: "<< res[doc].second << std::endl;
        }
    }

    void load_info(int &doc_amt, int &avg_doc_len){
        std::ifstream info_in;
        std::string info_name = "info";
        std::string line;
        info_in.open(info_name);
        getline(info_in, line);
        doc_amt = stoi(line);
        getline(info_in, line);
        avg_doc_len = stoi(line);
        info_in.close();
    }

    void search(std::string query){
        std::string query_copy = query;

        // file information
        std::ifstream indexing_in;
        std::ifstream posting_in;  
        std::string indexing_name = "indexing";
        std::string posting_name = "posting";
        
        // varaibles to store amount of documents that were indexed, and average document length. 
        int doc_amt;
        int avg_doc_len;
        load_info(doc_amt, avg_doc_len);

        // Details for posting list
        FILE * postings_file;
        postings_file = fopen ("posting" , "rb");
        uint64_t doc_id;
        uint16_t term_count;
        uint32_t doc_len;

        // Data structures to hold info, score for each docid and counts for each word in query.
        std::vector<unsigned long> info;
        std::unordered_map<uint64_t, float> query_results; 
        std::unordered_map<std::string, int> query_term_count;

        std::istringstream qss(query_copy);
        while (getline(qss, query_copy, ' ')){
            query_term_count[query_copy]++;
        }
        
        std::istringstream iss(query);
        indexing_in.open(indexing_name);

        while (std::getline(iss, query, ' ')){
            if(bsearch_indexing(query, indexing_in)){
                load_block_info(info, indexing_in);
                fseek(postings_file, info[BLOCK], SEEK_SET);
                int docs_read = 0;
                do{
                    fread(&doc_id, sizeof(uint64_t), 1, postings_file);
                    fread(&term_count, sizeof(uint16_t), 1, postings_file);
                    fread(&doc_len, sizeof(uint32_t), 1, postings_file);
                    query_results[doc_id] += bm25(doc_len, avg_doc_len, doc_amt, info[AMT], term_count, query_term_count[query]);
                    docs_read++;
                }while(docs_read < info[AMT]);
            }
            info.clear();
        }
        print_query_ranking(query_results);
        posting_in.close();
        indexing_in.close();
    }
}