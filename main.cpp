#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>
#include <map>
#include <algorithm>
#include <unordered_map>
#include <math.h>

using namespace std;

map<string, unordered_map<string, int> > dict;
unordered_map<string, int> doc_lengths;
ifstream infile;
ofstream index_out;
ofstream posting_out;
ofstream info_out;

inline void to_lower(string &s){
    transform(s.begin(), s.end(), s.begin(), ::tolower); 
}
// most word occeurnce in a doc AND longest doc.
void save_dict(){
    index_out.open("indexing");
    posting_out.open("posting");
    unsigned long block_loc = 0;
    int doc_size = 22;
    int most_occured = 0, longest_doc = 0, occurence_pad = 0, doc_len_pad = 0;
    vector<string> occurence_pad_dic{"", "00", "0", ""};
    vector<string> doc_pad_dic{"", "0000", "000", "00", "0", ""};
    for (auto it : dict) {
        index_out << "#" << it.first << " "; // << " ";
        //unordered_map<string, int> internal_map = it.second;
        for (auto it2: it.second) {
            //              Doc id.            term occurence      doc len
            if(it2.second > most_occured){
                most_occured = it2.second;
            }
            if(doc_lengths[it2.first] > longest_doc){
                longest_doc = doc_lengths[it2.first];
            }
            occurence_pad = ceil(log10(it2.second) + 0.000001);
            doc_len_pad = ceil(log10(doc_lengths[it2.first]) + 0.000001);

            posting_out << it2.first << " " << occurence_pad_dic[occurence_pad] << it2.second << " "  << doc_pad_dic[doc_len_pad] << doc_lengths[it2.first];
            posting_out << endl;
        }
        index_out << it.second.size() <<  " " << block_loc << endl;
        block_loc += (it.second.size() * doc_size);
    }
    cout << "most occurence: " << most_occured << " longest doc: " << longest_doc << endl;
    index_out.close();
    posting_out.close();
}

void save_info(){
    info_out.open("info");
    unsigned long total = 0;
    for(auto len : doc_lengths){
        total += len.second;
    }
    double avg_doc_len = total / doc_lengths.size();
    info_out << doc_lengths.size() << endl;
    info_out << avg_doc_len;
    info_out.close();
}

void get_input(){
    string line;
    string curr_doc;
    int done;
    int first_cut, second_cut, diff;
    // regex word("\\w+(\\.?-?\\w+)*");
    regex xml_token("\\</?.+\\>");
    regex doc_header("^WSJ(\\d+-\\d+)$");
    infile.open("wsj.small.xml");
    if(infile.is_open()){
        while(getline(infile, line)){ // for line in file
            istringstream iss(line);
            while (getline(iss, line, ' ')){ // for word in line
                if(line.length() == 0 || (line.front() == '<' && line.back() == '>')){
                    continue;
                }
                if(regex_match(line, doc_header)){
                    curr_doc = line.substr(3, line.size());
                    to_lower(line);
                    dict[line][curr_doc]++;
                    done++;
                    if(done % 1000 == 0){
                        cout << done << endl;
                    }
                    continue;
                }
                else if(line.front() != '<' && line.back() == '>'){
                    line = line.substr(0, line.find('<'));
                }
                else if(line.front() == '<' && line.back() != '>'){
                    line = line.substr(line.find('>') + 1);
                }
                // else if(line.back() == ',' || line.back() == '.'){
                //     line = line.substr(0, line.size() - 1);
                // }
                else{
                    //cout << line << endl;
                    first_cut = 0;
                    second_cut = line.length();
                    for(int i = 0; i < line.length(); i++) {
                        if(!isalnum(line[i])){
                            first_cut++;
                        }else{
                            break;
                        }
                    }
                    for(int i = line.length() - 1; i >= 0; i--) {
                        if(!isalnum(line[i])){
                            second_cut--;
                        }else{
                            break;
                        }
                    }
                    if(first_cut == line.length() || second_cut == 0){
                        //cout << line << endl;
                        continue;
                    }else{
                        diff = line.length() - second_cut;
                        line.erase(0, first_cut);
                        line.erase(second_cut - first_cut, diff);
                    }
                }
                // !regex_match(line, xml_token)
                to_lower(line);
                dict[line][curr_doc]++;
                doc_lengths[curr_doc]++;
            }
        }
        infile.close();
    }
}

void help(){
    cerr << "USAGE: ./search" << " <option> <file>\n"
        << "OptionsL\n"
        << "-i wsj.xml, -- to preform indexing on the data\n"
        << "-s query, -- to search for a query" << endl;
}

int main(int argc, char* argv[]){
    // if(argc <= 1){
    //     help();
    //     return EXIT_FAILURE;
    // }
    // string option = argv[1];
    // if(option == "-i"){
    //     // get input
    //     // save to disk
    // }else if(option == "-s" && argc == 3){
    //     // take input
    //     // do query
    // }else{
    //     help();
    //     return EXIT_FAILURE;
    // }
    cout << "getting input" << endl;
    get_input();
    cout << "saving dict" << endl;
    save_dict();
    cout << "saving info" << endl;
    save_info();
    return 0;
}