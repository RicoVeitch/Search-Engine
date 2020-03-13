#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <vector>
#include <math.h>

#define AMT 0
#define BLOCK 1
#define TERM_FREQ 0
#define DOC_LEN 1

using namespace std;

ifstream indexing_in;
ifstream posting_in;
ifstream info_in;
string indexing_name = "indexing";
string posting_name = "posting";
string info_name = "info";
float k1 = 1.2;
int k3 = 7;
float b = 0.75;

long doc_amt;
double avg_doc_length;

void load_block_info(vector<unsigned long> &res){
    string line;
    unsigned long temp;
    getline(indexing_in, line);
    istringstream iss(line);
    while (getline(iss, line, ' ')){
        if(line == "" || line == " ")
            continue;
        temp = stoul(line, nullptr, 0);
        res.push_back(temp);
    }
} 

void load_posting_info(vector<int> &res){
    string line;
    int temp;
    getline(posting_in, line);
    istringstream iss(line);
    while (getline(iss, line, ' ')){
        if(line == "" || line == " ")
            continue;
        temp = stoi(line);
        res.push_back(temp);
    }
}

string walk_back(unsigned long mid){
    string word;
    indexing_in >> word;
    while(word[0] != '#'){
        mid--;
        indexing_in.seekg(mid);
        indexing_in >> word;
    }
    return word.substr(1, word.size());
}

bool bsearch_indexing(string &target_term){
    unsigned long left = 0, right = 0, mid = 0;
    string word;
    bool found = false;

    if(!indexing_in.is_open()){
        cerr << "File closed" << endl;
    }
    indexing_in.seekg(0, indexing_in.beg);
    left = indexing_in.tellg();
    indexing_in.seekg(0, indexing_in.end);
    right = indexing_in.tellg();

    while(left <= right){
        mid = left + (right - left) / 2;
        indexing_in.seekg(mid);
        word = walk_back(mid);
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
    float wt = log2((N - nt + 0.5) / (nt + 0.5));
    float second = ((k1 + 1) * tf) / (K + tf);
    float third = ((k3 + 1) * tq) / (k3 + tq);
    return wt * second * third;
}

void print_query_ranking(map<string, float> &query_results){
    for(auto score : query_results){
        cout << score.first << ": " << score.second << endl;
    }
}

int main(){
    string query = "john blair account";
    string query_copy = query;

    string doc_id;

    string line;
    int doc_amt;
    int avg_doc_len;
    int block_size = 22;
    info_in.open(info_name);
    getline(info_in, line);
    doc_amt = stoi(line);
    getline(info_in, line);
    avg_doc_len = stoi(line);
    //cout << "doc amnt: " << doc_amt << " avg_doc_len: " << avg_doc_len << endl;
    vector<unsigned long> info;
    vector<int> posting_info;

    map<string, float> query_results; //  change to unordered_map later

    unordered_map<string, int> query_term_count;
    istringstream qss(query_copy);
    while (getline(qss, query_copy, ' ')){
        cout << query_copy << endl;
        query_term_count[query_copy]++;
    }
    cout << "------" << endl;
    
    istringstream iss(query);
    indexing_in.open(indexing_name);
    posting_in.open(posting_name);
    while (getline(iss, query, ' ')){
        cout << query << endl;
        if(bsearch_indexing(query)){
            load_block_info(info);
            //cout << info[AMT] << " " << info[BLOCK] << endl;
            
            posting_in.seekg(info[BLOCK]);
            for(int doc_non = 1; doc_non <= info[AMT]; doc_non++){
                posting_in >> doc_id;
                load_posting_info(posting_info);
                cout << doc_id << endl;
                // cout << doc_id << " " << posting_info[TERM_FREQ] << " " << posting_info[DOC_LEN] << " "  << avg_doc_len << " " << 
                // doc_amt << " " << info[AMT] << endl;
                query_results[doc_id] += bm25(posting_info[DOC_LEN], avg_doc_len, doc_amt, info[AMT], posting_info[TERM_FREQ], query_term_count[query]);
                posting_in.seekg(info[BLOCK] + (block_size * doc_non));
                posting_info.clear();
            }
        }else{
            cout << query << " not found" << endl;
        }
        info.clear();
    }
    print_query_ranking(query_results);
    posting_in.close();
    indexing_in.close();
    return EXIT_SUCCESS;
}