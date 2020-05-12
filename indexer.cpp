#include "indexer.h"

namespace SE{
    void save_dict(std::map<std::string, std::unordered_map<uint64_t, unsigned short> > &dict, std::unordered_map<uint64_t, uint32_t> &doc_lengths){
        std::ofstream index_out;
        std::ofstream posting_out;
        index_out.open("indexing");
        posting_out.open("posting", std::ofstream::binary);
        unsigned long block_loc = 0;
        const size_t doc_size = 14;
        int most_occured = 0, longest_doc = 0, occurence_pad = 0, doc_len_pad = 0;

        for (auto it : dict) {
            index_out << "~" << it.first << " "; // << " ";
            //unordered_map<string, int> internal_map = it.second;
            for (auto it2: it.second) {
                //              Doc id.            term occurence      doc len
                if(it2.second > most_occured){
                    most_occured = it2.second;
                }
                if(doc_lengths[it2.first] > longest_doc){
                    longest_doc = doc_lengths[it2.first];
                }

                posting_out.write((char*)&it2.first, sizeof(it2.first));
                posting_out.write((char*)&it2.second, sizeof(it2.second));
                posting_out.write((char*)&doc_lengths[it2.first], sizeof(doc_lengths[it2.first])); // CHECK THIS

            }
            index_out << it.second.size() <<  " " << block_loc << "\n";
            block_loc += (it.second.size() * doc_size);
        }
        // std::cout << "most occurence: " << most_occured << " longest doc: " << longest_doc << std::endl;
        index_out.close();
        posting_out.close();
    }

    void save_info(std::unordered_map<uint64_t, uint32_t> &doc_lengths){
        std::ofstream info_out;
        info_out.open("info");
        unsigned long total = 0;
        for(auto len : doc_lengths){
            total += len.second;
        }
        double avg_doc_len = (double) total / doc_lengths.size();
        info_out << doc_lengths.size() << "\n";
        info_out << avg_doc_len;
        info_out.close();
    }

    void clean_string(std::string &line, std::vector<std::string> &res){
        int start = 0;
        std::string token;
        for(int c = 0; c < line.length(); c++){
            if(!std::isalnum(line[c])){
                if(line[c] == '%'){ 
                    res.push_back("%");
                } else if(line[c] == '.' && (c != 0 && c != line.length() - 1) && (std::isalnum(line[c-1]) && std::isalnum(line[c+1]))){ // for decimal
                    continue;
                }
                token = line.substr(start, c-start);
                if(token.length() > 0){
                    to_lower(token);
                    res.push_back(token);
                }
                start = c + 1;
            }
        }
        token = line.substr(start, line.length()-start);
        if(token.length() > 0){
            to_lower(token);
            res.push_back(token);
        }
    }

    void parse(std::map<std::string, std::unordered_map<uint64_t, unsigned short> > &dict, std::unordered_map<uint64_t, uint32_t> &doc_lengths){
        std::string line;
        std::string curr_doc;
        int done = 0;
        int first_cut, second_cut, diff;
        uint64_t doc_id;
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
                        curr_doc.erase(6, 1);
                        doc_id = static_cast<uint64_t>(std::stoull(curr_doc));
                        to_lower(line);
                        dict[line][doc_id]++;
                        done++;
                        if(done % 1000 == 0){
                            std::cout << done << "\n";
                        }
                        continue;
                    }
                    else if(line.front() != '<' && line.back() == '>'){
                        line = line.substr(0, line.find('<'));
                    }
                    else if(line.front() == '<' && line.back() != '>'){
                        line = line.substr(line.find('>') + 1);
                    }

                    std::vector<std::string> clean_line;
                    clean_string(line, clean_line);
                    for(std::string word : clean_line){
                        if(stop_words.find(word) == stop_words.end()){
                            dict[word][doc_id]++;
                            doc_lengths[doc_id]++;
                        }
                    }
                }
            }
            infile.close();
        }
    }

    void index(){
        std::map<std::string, std::unordered_map<uint64_t, unsigned short> > dict; // word: doc_id: amount
        std::unordered_map<uint64_t, uint32_t> doc_lengths; // doc_id: doc_length
        std::cout << "PARSING" << std::endl;
        parse(dict, doc_lengths);
        std::cout << "SAVING DICT" << std::endl;
        save_dict(dict, doc_lengths);
        std::cout << "SAVING INFO" << std::endl;
        save_info(doc_lengths);
    }
}