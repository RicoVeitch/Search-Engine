#include "indexer.h"

namespace SE{

    inline void to_lower(std::string &s){
        transform(s.begin(), s.end(), s.begin(), ::tolower); 
    }
    // most word occeurnce in a doc AND longest doc.
    void save_dict(std::map<std::string, std::unordered_map<std::string, int> > &dict, std::unordered_map<std::string, int> &doc_lengths){
        std::ofstream index_out;
        std::ofstream posting_out;
        index_out.open("indexing");
        posting_out.open("posting");
        unsigned long block_loc = 0;
        int doc_size = 22;
        int most_occured = 0, longest_doc = 0, occurence_pad = 0, doc_len_pad = 0;
        std::vector<std::string> occurence_pad_dic{"", "00", "0", ""};
        std::vector<std::string> doc_pad_dic{"", "0000", "000", "00", "0", ""};
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
                posting_out << std::endl;
            }
            index_out << it.second.size() <<  " " << block_loc << std::endl;
            block_loc += (it.second.size() * doc_size);
        }
        std::cout << "most occurence: " << most_occured << " longest doc: " << longest_doc << std::endl;
        index_out.close();
        posting_out.close();
    }

    void save_info(std::unordered_map<std::string, int> &doc_lengths){
        std::ofstream info_out;
        info_out.open("info");
        unsigned long total = 0;
        for(auto len : doc_lengths){
            total += len.second;
        }
        double avg_doc_len = total / doc_lengths.size();
        info_out << doc_lengths.size() << std::endl;
        info_out << avg_doc_len;
        info_out.close();
    }

    void parse(std::map<std::string, std::unordered_map<std::string, int> > &dict, std::unordered_map<std::string, int> &doc_lengths){
        std::string line;
        std::string curr_doc;
        int done;
        int first_cut, second_cut, diff;
        // regex word("\\w+(\\.?-?\\w+)*");
        std::regex xml_token("\\</?.+\\>");
        std::regex doc_header("^WSJ(\\d+-\\d+)$");
        std::ifstream infile;
        infile.open("wsj.xml");
        if(infile.is_open()){
            while(getline(infile, line)){ // for line in file
                std::istringstream iss(line);
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
                            std::cout << done << std::endl;
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

    void index(){
        std::map<std::string, std::unordered_map<std::string, int> > dict;
        std::unordered_map<std::string, int> doc_lengths;
        std::cout << "PARSING" << std::endl;
        parse(dict, doc_lengths);
        std::cout << "SAVING DICT" << std::endl;
        save_dict(dict, doc_lengths);
        std::cout << "SAVING INFO" << std::endl;
        save_info(doc_lengths);
    }
}