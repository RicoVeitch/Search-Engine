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
        while(word[0] != '#'){
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

    float bm25(int dl, float adl, int N, long nt, int tf, int tq){
        float K = k1 * ((1-b) + ((b * dl) / adl));
        // float wt = log2((N - nt + 0.5) / (nt + 0.5));
        // float second = ((k1 + 1) * tf) / (K + tf);
        // float third = ((k3 + 1) * tq) / (k3 + tq);
        // return wt * second * third;
        return log2((N - nt + 0.5) / (nt + 0.5)) * ((k1 + 1) * tf) / (K + tf) * ((k3 + 1) * tq) / (k3 + tq);
    }

    void print_query_ranking(std::unordered_map<std::string, float> &query_results){
        int max_docs = 10;
        std::vector<std::pair<std::string, float> > res;
        std::copy(query_results.begin(), query_results.end(), std::back_inserter<std::vector<std::pair<std::string, float>>>(res));
        sort(res.begin(), res.end(),
                [](const std::pair<std::string, float>& l, std::pair<std::string, float>& r) {
                    if (l.second != r.second)
                        return l.second > r.second;

                    return l.first < r.first;
                });
        for(int doc = 0; doc < max_docs && doc < res.size(); doc++){
            std::cout << doc+1 << ": WSJ" << res[doc].first << "\tscore: "<< res[doc].second << std::endl;
        }
    }

    void search(std::string query){
        clock_t start = clock();
        // std::string query = "john blair in the record out of sight while tipping over";
        std::string query_copy = query;

        std::ifstream indexing_in;
        std::ifstream posting_in;
        std::ifstream info_in;
        std::string indexing_name = "indexing";
        std::string posting_name = "posting";
        std::string info_name = "info";

        std::string doc_id;
        int occurences, doc_len;
        
        std::string line;
        int doc_amt;
        int avg_doc_len;
        int block_size = 22;
        info_in.open(info_name);
        getline(info_in, line);
        doc_amt = stoi(line);
        getline(info_in, line);
        avg_doc_len = stoi(line);

        /*******/
        char buffer[block_size];
        FILE * postings_file;
        postings_file = fopen ("posting" , "r");
        /*******/

        std::vector<unsigned long> info;
        std::vector<int> posting_info;

        std::unordered_map<std::string, float> query_results; 

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
                //posting_in.seekg(info[BLOCK]);
                fseek(postings_file, info[BLOCK], SEEK_SET);
                int doc_non = 1;
                while(fgets(buffer , block_size , postings_file) != NULL && doc_non <= info[AMT]){
                    if(buffer[0] != '\n'){
                        //cout << buffer << endl;
                        std::string line(buffer);
                        doc_id = line.substr(0, 11);
                        occurences = stoi(line.substr(12, 3));
                        doc_len = stoi(line.substr(16, 5));
                        //cout << doc_id << " " << occurences << " " << doc_len << endl;
                        query_results[doc_id] += bm25(doc_len, avg_doc_len, doc_amt, info[AMT], occurences, query_term_count[query]);
                        doc_non++;
                    }
                }
            }
            info.clear();
        }
        print_query_ranking(query_results);
        posting_in.close();
        indexing_in.close();
        double duration = (clock() - start ) / (double) CLOCKS_PER_SEC;
        std::cout << duration << std::endl;
    }
}