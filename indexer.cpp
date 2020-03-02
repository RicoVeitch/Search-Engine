#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <regex>
#include <sstream>

using namespace std;

void print_dict(unordered_map<string, unordered_map<string, int> >& dict){
    for (auto it : dict) {
        cout << it.first << " : ";
        unordered_map<string, int> internal_map = it.second;
        for (auto it2: internal_map) {
            //if (it2 != internal_map.begin())
            cout << it2.first << ":" << it2.second;
            cout << " , ";
        }
        cout << endl;
    }
}

int main(){
    unordered_map<string, unordered_map<string, int> > dict; 
    string line;
    string curr_doc = "";
    regex doc_header("^WSJ(\\d+-\\d+)$");
    fstream infile("wsj.clean.txt");
    string temp = "WSJ870324-0001";
    if(infile.is_open()){
        while(getline(infile, line)){ // for line in file
            cout << line << endl;
            if(regex_match(line, doc_header)){
                curr_doc = line;
                dict[line][curr_doc]++;
            }else{
                istringstream iss(line);
                while (getline(iss, line, ' ')){
                    dict[line][curr_doc]++;
                }
            }
        }
        infile.close();
    }
    print_dict(dict);
    return 0;
}