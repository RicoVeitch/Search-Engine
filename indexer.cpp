#include "indexer.h"
#include "shared_const.h"

namespace SE{
    const uint8_t Indexer::POST_ENTRY_SIZE = 14;
    const std::unordered_set<std::string> Indexer::stop_words({"i", "me", "my", "myself", "we", "our", "ours", "ourselves", "you", "your", "yours", "yourself", 
    "yourselves", "he", "him", "his", "himself", "she", "her", "hers", "herself", "it", "its", "itself", "they", "them", "their", "theirs", 
    "themselves", "what", "which", "who", "whom", "this", "that", "these", "those", "am", "is", "are", "was", "were", "be", "been", "being", 
    "have", "has", "had", "having", "do", "does", "did", "doing", "a", "an", "the", "and", "but", "if", "or", "because", "as", "until", "while", 
    "of", "at", "by", "for", "with", "about", "against", "between", "into", "through", "during", "before", "after", "above", "below", "to", "from", 
    "up", "down", "in", "out", "on", "off", "over", "under", "again", "further", "then", "once", "here", "there", "when", "where", "why", "how", 
    "all", "any", "both", "each", "few", "more", "most", "other", "some", "such", "no", "nor", "not", "only", "own", "same", "so", "than", "too", 
    "very", "s", "t", "can", "will", "just", "don", "should", "now"});

    void Indexer::save_dict(std::map<std::string, std::unordered_map<uint64_t, unsigned short> > &dict, std::unordered_map<uint64_t, uint32_t> &doc_lengths){
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

    void Indexer::save_info(std::unordered_map<uint64_t, uint32_t> &doc_lengths){
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

    char *Indexer::clean_token(){
        while(!isalnum(*curr) && *curr != '<' && *curr != '\0'){
                curr++;
        }
        char *start = curr;
        if(isalnum(*curr)){
            while(isalnum(*curr) || *curr == '-'){
                curr++;
            }
        }else if(*curr == '<'){
            while(*curr != '>'){
                curr++;
            }
            curr++;
        }else{
            return NULL;
        }
        memcpy(token_buffer, start, curr - start);
        token_buffer[curr - start] = '\0';
        return token_buffer;
    }

    void Indexer::parse(std::string wsj_path, std::map<std::string, std::unordered_map<uint64_t, unsigned short> > &dict, std::unordered_map<uint64_t, uint32_t> &doc_lengths){
        std::string curr_doc;
        std::string token;
        char *raw_token; 
        int done = 0;
        uint64_t doc_id; 
        bool is_docid = false;

        FILE *fp;
        fp = fopen(wsj_path.c_str(), "r");
        while (fgets(buffer, sizeof(buffer), fp) != NULL){
        //cout << buffer << endl;
            curr = buffer;
            while((raw_token = clean_token()) != NULL){
                if(strcmp(raw_token, "<DOCNO>") == 0){
                    is_docid = true;
                }else if(is_docid){
                    token = std::string(raw_token);
                    curr_doc = token.substr(3, token.size());
                    curr_doc.erase(6, 1);
                    doc_id = static_cast<uint64_t>(std::stoull(curr_doc));
                    done++;
                    if(done % 1000 == 0){
                        std::cout << done << "\n";
                    }
                    is_docid = false;
                }
                else if(raw_token[0] != '<'){
                    token = std::string(raw_token);
                    to_lower(token);
                    if(stop_words.find(token) == stop_words.end()){
                        dict[token][doc_id]++;
                        doc_lengths[doc_id]++;
                    }
                }
            }
        }

    }

    void Indexer::index(std::string wsj_path){
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