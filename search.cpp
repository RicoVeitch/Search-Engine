#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <algorithm>
#include <unordered_map>

using namespace std;

ifstream infile;

void bsearch_file(){
    return;
}

string walk_back(unsigned long mid){
    string val;
    infile >> val;
    while(val[0] != '#'){
        mid--;
        infile.seekg(mid);
        infile >> val;
    }
    return val.substr(1, val.size());
}

int main(){
    string infile_name = "wsj_clean";
    unsigned long left = 0, right = 0, mid = 0;
    string word;
    string term = "about"; // italy, major, offers, telegraph, james, about
    bool found = false;
    infile.open(infile_name);

    if(!infile.is_open()){
        cerr << "Unable to open file" << endl;
    }
    infile.seekg(0, infile.beg);
    left = infile.tellg();
    infile.seekg(0, infile.end);
    right = infile.tellg();

    while(left <= right){
        mid = left + (right - left) / 2;
        infile.seekg(mid);
        word = walk_back(mid);
        cout << "mid: " << mid << " word: " << word << endl;
        if(word > term){
            right = mid - 1;
        }else if(word < term){
            left = mid + 1;
        }else{
            found = true;
            break;
        }
    }
    if(found){
        cout << "FOUND" << endl;
        cout << "word: " << word << endl;
    }
    string line;
    while(getline(infile, line)){
        cout << line << endl;
        break;
    }
    return EXIT_SUCCESS;
}