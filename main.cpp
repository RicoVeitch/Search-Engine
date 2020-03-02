#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <regex>

using namespace std;

int main(){
    string line;
    string outline = "";
    bool is_tokens = false;
    bool end_doc = false;
    // regex word("\\w+(\\.?-?\\w+)*");
    regex xml_token("\\</?.+\\>");
    ifstream infile("wsj.small.xml");
    ofstream outfile("wsj.clean.txt");
    if(infile.is_open()){
        while(getline(infile, line)){ // for line in file
            istringstream iss(line);
            while (getline(iss, line, ' ')){ // for word in line
                if(line == ""){
                    continue;
                }
                else if(line.front() != '<' && line.back() == '>'){
                    //outfile << line.substr(0, line.find('<')) << " ";
                    outline.append(line.substr(0, line.find('<')));
                    outline.append(" ");
                    is_tokens = true;
                }
                else if(line.front() == '<' && line.back() != '>'){
                    //outfile << line.substr(line.find('>') + 1, line.size()) << " ";
                    outline.append(line.substr(line.find('>') + 1, line.size()));
                    outline.append(" ");
                    is_tokens = true;
                }
                else if(!regex_match(line, xml_token)){
                    outline.append(line);
                    outline.append(" ");
                    //outfile << line << " ";
                    is_tokens = true;
                }
                else if(line == "</DOC>"){
                    end_doc = true;
                }
            }
            if(is_tokens || end_doc){
                outline.pop_back();
                outfile << outline;
                outfile << endl;
                outline = "";
                is_tokens = false;
                end_doc = false;
            }
        }
        infile.close();
    }
    return 0;
}