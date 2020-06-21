#include "search.h"
#include "shared_const.h"

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
            std::string word;
            word.resize(MAX_WORD_LEN);
            mid = left + (right - left) / 2;
            mid -= (mid % INDEX_ENTRY_SIZE);
            indexing_in.seekg(mid);
            indexing_in.read(&word[0], 64);
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

    std::pair<uint32_t, uint32_t> Search::bsearch_range(uint64_t target_doc, std::ifstream &range_in, bool header){
        std::cout << target_doc << std::endl;
        uint32_t left = 0, right = 0, mid = 0; 
        uint64_t doc_id;
        std::pair<uint32_t, uint32_t> result;
        size_t line_length = (sizeof(uint32_t) * 4) + sizeof(target_doc);
        range_in.seekg(0, range_in.beg);
        left = range_in.tellg();
        range_in.seekg(0, range_in.end);
        right = range_in.tellg();
        while(left <= right){
            mid = left + (right - left) / 2;
            std::cout << "before: "<< mid << std::endl;
            mid -= (mid % line_length); // round so mid is pointing to the start of an entry.
            std::cout << "after: "<< mid << std::endl;
            range_in.seekg(mid);
            range_in.read((char*)&doc_id, sizeof(target_doc));
            std::cout << doc_id << std::endl;
            if(doc_id > target_doc){
                right = mid - line_length;
            } else if(doc_id < target_doc){
                left = mid + line_length;
            } else{
                break;
            }
        }
        if(header){
            range_in.seekg(mid + ((sizeof(uint32_t) * 2) + sizeof(target_doc))); // skip to the header
        }
        range_in.read((char*)&result.first, sizeof(uint32_t));
        range_in.read((char*)&result.second, sizeof(uint32_t));
        return result;
    }

    std::string Search::get_doc_content(uint64_t doc_id, bool header){
        std::ifstream range_in;
        std::ifstream wsj_in;
        std::pair<uint32_t, uint32_t> loc;
        uint32_t doc_len;
        std::string output;
        
        range_in.open(range_name, std::ifstream::binary);
        loc = bsearch_range(doc_id, range_in, header);
        doc_len = loc.second - loc.first;
        std::cout << doc_len << " " << loc.first << " " << loc.second << "\n";
        char* buffer = new char[doc_len + 1];
        wsj_in.open("wsj.xml", std::ifstream::binary);
        wsj_in.seekg(loc.first);
        wsj_in.read(buffer, doc_len);
        buffer[doc_len] = '\0';
        output = std::string(buffer);
        delete[] buffer;

        if(header){
            for(int i = 0; i < (int) output.size(); i++){
                if(output[i] == '\n'){
                    output[i] = ' ';
                }
            }
        }
        return output;
    }

    double Search::bm25(uint32_t dl, float adl, int N, uint32_t nt, uint16_t tf, int tq){
        double K = k1 * ((1-b) + ((b * dl) / adl));
        return log2((N - nt + 0.5) / (nt + 0.5)) * ((k1 + 1) * tf) / (K + tf) * ((k3 + 1) * tq) / (k3 + tq);
    }

    std::vector<uint64_t> Search::print_query_ranking(std::unordered_map<uint64_t, double> &query_results){
        int max_docs = 10;
        //std::string output = "";
        std::vector<uint64_t> output;
        std::vector<std::pair<uint64_t, double> > res;
        std::copy(query_results.begin(), query_results.end(), std::back_inserter<std::vector<std::pair<uint64_t, double>>>(res));
        sort(res.begin(), res.end(),
                [](const std::pair<uint64_t, double>& l, std::pair<uint64_t, double>& r) {
                    if (l.second != r.second)
                        return l.second > r.second;

                    return l.first < r.first;
                });
        std::string filename;
        int cap = std::min((int) res.size(), 10);
        for(int doc = 0; doc < cap; doc++){
            // filename = std::to_string(res[doc].first);
            // // filename.insert(6, 1, '-');
            // output += filename + "\n"; // "WSJ" + 
            output.push_back(res[doc].first);
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

    std::vector<uint64_t> Search::search(std::string query){
        if(query.empty()){
            return {};
        }
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
            return {};
        }
        return print_query_ranking(query_results);
    }
}