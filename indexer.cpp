#include "indexer.h"
#include "shared_const.h"

namespace SE{
    void save_dict(std::map<std::string, std::unordered_map<uint64_t, unsigned short> > &dict, std::unordered_map<uint64_t, uint32_t> &doc_lengths){
        std::ofstream index_out;
        std::ofstream posting_out;
        index_out.open(indexing_name, std::ofstream::binary);
        posting_out.open(posting_name, std::ofstream::binary);
        uint64_t block_loc = 0;
        uint32_t longest_doc = 0;
        std::string word;

        for (auto it : dict) {
            word = it.first;
            word.resize(MAX_WORD_LEN);
            index_out.write(word.c_str(), word.size());
            for (auto it2: it.second) {
                longest_doc = std::max(longest_doc, doc_lengths[it2.first]);
                posting_out.write((char*)&it2.first, sizeof(it2.first));
                posting_out.write((char*)&it2.second, sizeof(it2.second));
                posting_out.write((char*)&doc_lengths[it2.first], sizeof(doc_lengths[it2.first]));

            }
            uint32_t word_count = it.second.size();
            index_out.write((char*)&word_count, sizeof(word_count));
            index_out.write((char*)&block_loc, sizeof(block_loc));
            block_loc += (word_count * POST_ENTRY_SIZE);
        }
        index_out.close();
        posting_out.close();
    }

    void save_info(std::unordered_map<uint64_t, uint32_t> &doc_lengths){
        std::ofstream info_out;
        info_out.open(info_name);
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

    void parse(std::string wsj_path, std::map<std::string, std::unordered_map<uint64_t, unsigned short> > &dict, std::unordered_map<uint64_t, uint32_t> &doc_lengths){
        std::string line;
        std::string curr_doc;
        int done = 0;
        int first_cut, second_cut, diff;
        uint64_t doc_id;
        // regex word("\\w+(\\.?-?\\w+)*");
        std::regex xml_token("\\</?.+\\>");
        std::regex doc_header("^WSJ(\\d+-\\d+)$");
        std::ifstream infile;
        infile.open(wsj_path);
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
                        // to_lower(line);
                        //dict[line][doc_id]++;
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
        }else{
            std::cout << "wsj.xml not found in provided path\n";
        }
    }

    void index(std::string wsj_path){
        std::map<std::string, std::unordered_map<uint64_t, unsigned short> > dict; // word: doc_id: amount
        std::unordered_map<uint64_t, uint32_t> doc_lengths; // doc_id: doc_length
        std::cout << "PARSING" << std::endl;
        parse(wsj_path, dict, doc_lengths);
        std::cout << "SAVING DICT" << std::endl;
        save_dict(dict, doc_lengths);
        std::cout << "SAVING INFO" << std::endl;
        save_info(doc_lengths);
    }
}