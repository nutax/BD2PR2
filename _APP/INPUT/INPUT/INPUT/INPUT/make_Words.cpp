#include <set>
#include <array>
#include <string>
#include <fstream>
#include <cstdint>
#include <assert.h>

using char_t = char;
constexpr static std::uint8_t FixedWord_sz = 24;
constexpr static std::uint32_t FixedWord_bytes = 24*sizeof(char_t);
using FixedWord = std::array<char_t, FixedWord_sz>;

int main(void){
        
        //Iterar por todos los archivos y guardar todas las palabras en un std::set
        std::set<FixedWord> all_words;
        {
                std::fstream file_doc_names("INPUT/INPUT/RawDocs/Names/names.txt", std::fstream::in);
                assert(file_doc_names.is_open());

                std::string doc_name;
                std::ifstream doc;
                FixedWord word;
                for(std::uint32_t doc_id = 0; std::getline(file_doc_names, doc_name); ++doc_id){
                        if(doc_name == "") break;

                        doc.open(std::string{"INPUT/PreproDocs/"} + doc_name);
                        assert(doc.is_open());

                        for(std::uint8_t j = 0; j<FixedWord_sz; ++j) word[j] = '\0';

                        std::uint8_t i = 0;
                        for(char_t c; doc.get(c);){
                                if(i<FixedWord_sz){
                                        if(c != ' '){
                                                word[i] = c;
                                                ++i;
                                        }else{
                                                if(i > 1) all_words.insert(word);
                                                for(std::uint8_t j = 0; j<i; ++j) word[j] = '\0';
                                                i = 0;
                                        }
                                }else{
                                        all_words.insert(word);
                                        for(std::uint8_t j = 0; j<i; ++j) word[j] = '\0';
                                        i = 0;
                                }
                        }
                        if(i > 1) all_words.insert(word);
                        doc.close();
                }
                file_doc_names.close();
        }

        //Crear el words file en base al set con todas las palabras (ordenadas)
        {
                std::fstream file_words("Words/words.dat", std::fstream::out | std::fstream::trunc | std::fstream::binary);
                assert(file_words.is_open());
                std::uint32_t empty_ptr = 0;
                for(auto& w : all_words){
                        file_words.write((char*) &w, FixedWord_bytes);
                }
                file_words.close();
        }

        return 0;
}