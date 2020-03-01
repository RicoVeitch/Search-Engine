#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>

using namespace std;

int main(){
    string line;
    bool is_tokens = false;
    bool end_doc = false;
    // regex word("\\w+(\\.?-?\\w+)*");
    regex xml_token("\\</?.+\\>");
    ifstream infile("wsj.small.xml");
    if(infile.is_open()){
        while(getline(infile, line)){ // for line in file
            istringstream iss(line);
            while (getline(iss, line, ' ')){ // for word in line
                if(line == ""){
                    continue;
                }
                else if(line.front() != '<' && line.back() == '>'){
                    cout << line.substr(0, line.find('<')) << " ";
                    is_tokens = true;
                }
                else if(line.front() == '<' && line.back() != '>'){
                    cout << line.substr(line.find('>') + 1, line.size()) << " ";
                    is_tokens = true;
                }
                else if(!regex_match(line, xml_token)){
                    cout << line << " ";
                    is_tokens = true;
                }
                else if(line == "</DOC>"){
                    end_doc = true;
                }
            }
            if(is_tokens || end_doc){
                cout << endl;
                is_tokens = false;
                end_doc = false;
            }
        }
        infile.close();
    }
    return 0;
}