#include <map>
#include <array>
#include <vector>
#include <string>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <assert.h>

using char_t = char;
constexpr static std::uint8_t FixedWord_sz = 24;
constexpr static std::uint32_t FixedWord_bytes = 24*sizeof(char_t);
using FixedWord = std::array<char_t, FixedWord_sz>;



struct WordFr{
        std::uint32_t word_id;
        std::uint32_t doc_id;
        std::uint32_t fr;
};

constexpr static std::uint16_t Block_sz = 1024/sizeof(WordFr);
struct Block{
        std::uint8_t current_size = 0;
        std::array<WordFr, Block_sz> arr;
};

bool get_word_id(std::vector<FixedWord>& all_words, const std::uint32_t& n_words, const FixedWord& word, std::uint32_t& word_id){
        std::uint32_t l = 0;
        std::uint32_t r = n_words;
        while (r >= l) {
                std::uint32_t mid = l + (r - l) / 2;

                if (all_words[mid] == word) {
                        word_id = mid;
                        return true;
                }
                else if (all_words[mid] < word) {
                        l = mid + 1;
                }
                else {
                        
                        r = mid - 1;
                }
        }
        return false;
}

int main(void){
        //Abrir archivos
        std::fstream file_doc_names;
        std::fstream file_words;
        std::fstream file_wordsfr;
        {
                file_doc_names.open("INPUT/INPUT/INPUT/RawDocs/Names/names.txt", std::fstream::in);
                assert(file_doc_names.is_open());

                file_words.open("INPUT/Words/words.dat", std::fstream::in | std::fstream::binary);
                assert(file_words.is_open());

                file_wordsfr.open("WordsFr/wordsfr.dat", std::fstream::out | std::fstream::trunc | std::fstream::binary);
                assert(file_wordsfr.is_open());
        }
        
        //Contar la cantidad de palabras
        std::uint32_t n_words;
        {
                file_words.seekg(0, std::ios::end);
                n_words = (static_cast<std::uint32_t>(file_words.tellg())+1)/FixedWord_bytes;
        }

        //Cargar palabras a vector y cerrar el archivo
        std::vector<FixedWord> all_words;
        {
                all_words.resize(n_words);
                file_words.seekg(0, std::ios::beg);
                for (std::uint32_t i = 0; i < n_words; i++) {
                        file_words.read((char*) &all_words[i], sizeof(FixedWord));
                }
                file_words.close();
        }
        
        //Appendear todos los wordsfr a file_wordsfr
        {
                std::string doc_name;
                std::ifstream doc;
                FixedWord word;
                for(std::uint32_t doc_id = 0; std::getline(file_doc_names, doc_name); ++doc_id){
                        
                        //std::cout<<"Lee el nombre del documento"<<std::endl;

                        //Guardar todas las frecuencias en un map
                        std::map<FixedWord, std::uint32_t> wordsfr;
                        {
                                if(doc_name == "") break;
                                doc.open(std::string{"INPUT/INPUT/PreproDocs/"} + doc_name);
                                assert(doc.is_open());

                                //std::cout<<"Abre el documento"<<std::endl;
                                
                                for(std::uint8_t j = 0; j<FixedWord_sz; ++j) word[j] = '\0';

                                std::uint8_t i = 0;
                                std::map<FixedWord, std::uint32_t>::iterator it;
                                for(char_t c; doc.get(c);){
                                        if(i<FixedWord_sz){
                                                if(c != ' '){
                                                        word[i] = c;
                                                        ++i;
                                                }else{
                                                        if(i > 1) {
                                                                it = wordsfr.find(word);
                                                                if(it != wordsfr.end()) it->second += 1; else wordsfr[word] = 1;
                                                        }
                                                        for(std::uint8_t j = 0; j<i; ++j) word[j] = '\0';
                                                        i = 0;
                                                }
                                        }else{
                                                it = wordsfr.find(word);
                                                if(it != wordsfr.end()) it->second += 1; else wordsfr[word] = 1;
                                                for(std::uint8_t j = 0; j<i; ++j) word[j] = '\0';
                                                i = 0;
                                        }
                                }
                                //std::cout<<"Lee todas las palabras del documento"<<std::endl;
                                if(i > 1) {
                                        it = wordsfr.find(word);
                                        if(it != wordsfr.end()) it->second += 1; else wordsfr[word] = 1;
                                }
                                //std::cout<<"Lee la ultima palabra del documento"<<std::endl;
                                doc.close();
                        }
                        
                        //Appendear por block (word_id, doc_id, frecuencia)
                        {
                                Block block;
                                for(auto& wf : wordsfr){
                                        std::uint32_t word_id; //get_word_id(file_words, n_words, wf.first);
                                        if(get_word_id(all_words, n_words, wf.first, word_id)){
                                                block.arr[block.current_size].word_id = word_id;
                                                block.arr[block.current_size].doc_id = doc_id;
                                                block.arr[block.current_size].fr = wf.second;
                                                block.current_size += 1;
                                                if(block.current_size >= Block_sz){
                                                        file_wordsfr.write((char*) &block, sizeof(Block));
                                                        block.current_size = 0;
                                                }
                                        }
                                }
                                if(block.current_size > 0){
                                        file_wordsfr.write((char*) &block, sizeof(Block));
                                        block.current_size = 0;
                                }
                        }
                }
        }
        

        //Cerrar archivos
        {
                file_wordsfr.close();
                file_doc_names.close();
        }

        return 0;
}