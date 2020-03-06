#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <vector>

#define AMT 0
#define BLOCK 1

using namespace std;

ifstream infile;
string infile_name = "indexing";

void load_block_info(vector<unsigned long> &res){
    string line;
    unsigned long temp;
    getline(infile, line);
    istringstream iss(line);
    while (getline(iss, line, ' ')){
        if(line == "")
            continue;
        temp = stoul(line, nullptr, 0);
        res.push_back(temp);
    }
} 

string walk_back(unsigned long mid){
    string word;
    infile >> word;
    while(word[0] != '#'){
        mid--;
        infile.seekg(mid);
        infile >> word;
    }
    return word.substr(1, word.size());
}

bool bsearch_file(string &target_term){
    unsigned long left = 0, right = 0, mid = 0;
    string word;
    bool found = false;

    if(!infile.is_open()){
        cerr << "File closed" << endl;
    }
    infile.seekg(0, infile.beg);
    left = infile.tellg();
    infile.seekg(0, infile.end);
    right = infile.tellg();

    while(left <= right){
        mid = left + (right - left) / 2;
        infile.seekg(mid);
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

int main(){
    string query = "about a john";
    istringstream iss(query);
    vector<unsigned long> info;
    infile.open(infile_name);
    while (getline(iss, query, ' ')){
        if(bsearch_file(query)){
            load_block_info(info);
            cout << info[AMT] << " " << info[BLOCK] << endl;
        }else{
            cout << query << " not found" << endl;
        }
        info.clear();
    }
    infile.close();
    return EXIT_SUCCESS;
}