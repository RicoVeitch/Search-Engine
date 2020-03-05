#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <map>
#include <algorithm>
#include <unordered_map>

using namespace std;

map<string, unordered_map<string, int> > dict;
ifstream infile("wsj.small.xml");
ofstream outfile("wsj_clean");

inline void to_lower(string& s){
    transform(s.begin(), s.end(), s.begin(), ::tolower); 
}

void save_dict(){
    unsigned long block_loc = 0;
    int doc_size = 12;
    for (auto it : dict) {
        outfile << "#" << it.first << " "; // << " ";
        unordered_map<string, int> internal_map = it.second;
        // for (auto it2: internal_map) {
        //     //if (it2 != internal_map.begin())
        //     outfile << it2.first << " ";
        // }
        outfile << internal_map.size() <<  " " << block_loc << endl;
        block_loc += (internal_map.size() * doc_size);
    }
}

void get_input(){
    string line;
    string curr_doc;
    // regex word("\\w+(\\.?-?\\w+)*");
    regex xml_token("\\</?.+\\>");
    regex doc_header("^WSJ(\\d+-\\d+)$");

    if(infile.is_open()){
        while(getline(infile, line)){ // for line in file
            istringstream iss(line);
            while (getline(iss, line, ' ')){ // for word in line
                if(line == ""){
                    continue;
                }
                if(regex_match(line, doc_header)){
                    curr_doc = line.substr(3, line.size());
                    to_lower(line);
                    dict[line][curr_doc]++;
                    continue;
                }
                else if(line.front() != '<' && line.back() == '>'){
                    line = line.substr(0, line.find('<'));
                }
                else if(line.front() == '<' && line.back() != '>'){
                    line = line.substr(line.find('>') + 1);
                }
                else if(line.back() == ',' || line.back() == '.'){
                    line = line.substr(0, line.size() - 1);
                }
                if(!regex_match(line, xml_token)){
                    to_lower(line);
                    dict[line][curr_doc]++;
                }
            }
        }
        infile.close();
    }
}

int main(){
    get_input();
    save_dict();
    return 0;
}