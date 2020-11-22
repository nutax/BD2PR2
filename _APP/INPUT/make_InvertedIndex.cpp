#include <array>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <assert.h>
#include <cmath>
#include <string>
#include <vector>


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

struct Fr{
        std::uint32_t doc_id;
        std::uint32_t fr;
};

int main(void){
        //abrir archivo
        std::fstream file_index;
        std::fstream file_data;
        std::fstream file_orderedwordsfr;
        std::fstream file_words;
        {
            file_index.open("InvertedIndex/index.txt" , std::fstream::in | std::fstream::out | std::fstream::trunc);
            assert(file_index.is_open());

            file_data.open("InvertedIndex/data.dat" , std::fstream::in | std::fstream::out | std::fstream::trunc | std::fstream::binary);
            assert(file_data.is_open());

            file_orderedwordsfr.open("INPUT/OrderedWordsFr/orderedwordsfr.dat" , std::fstream::binary | std::fstream::in);
            assert(file_orderedwordsfr.is_open());

            file_words.open("INPUT/INPUT/INPUT/Words/words.dat", std::fstream::in | std::fstream::binary);
            assert(file_words.is_open());
        }


        //Contar la cantidad de blocks y palabras
        std::uint32_t n_blocks;
        std::uint32_t n_words;
        {
                file_orderedwordsfr.seekg(0, std::ios::end);
                n_blocks = (static_cast<std::uint32_t>(file_orderedwordsfr.tellg())+1)/sizeof(Block);

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

        //Iterar por todos los blocks y crear el indice invertido
        {
                
                file_orderedwordsfr.seekg(0, std::ios::beg);
                file_index.seekg(0, std::ios::beg);
                file_data.seekg(0, std::ios::beg);

                Block aux;
                std::vector<Fr> frs;
                std::uint32_t last_word_id = 0;
                std::uint32_t current_word_id = 0;
                for(std::uint32_t i = 0; i < n_blocks; ++i){
                        file_orderedwordsfr.read((char*) &aux, sizeof(Block));
                        for (std::uint32_t j = 0; j < aux.current_size; ++j){
                                current_word_id = aux.arr[j].word_id;
                                if(last_word_id != current_word_id){
                                        for(const auto& c : all_words[last_word_id]) {if(c == '\0') break; file_index<<c;}
                                        file_index<<','<<static_cast<std::uint32_t>(file_data.tellg())<<std::endl;

                                        std::uint32_t vec_sz = static_cast<std::uint32_t>(frs.size());

                                        std::uint32_t total_fr = 0;
                                        for(auto& fr : frs) total_fr += fr.fr;

                                        const std::uint32_t idf = static_cast<std::uint32_t>(1000*log10(static_cast<double>(1 + (total_fr/vec_sz))));
                                        file_data.write((char*) &idf, sizeof(std::uint32_t));

                                        file_data.write((char*) &vec_sz, sizeof(std::uint32_t));
                                        for(auto& fr : frs) {
                                                const double tf = log10(static_cast<double>(1+fr.fr));
                                                const std::uint32_t tfidf = static_cast<std::uint32_t>(tf*static_cast<double>(idf));
                                                fr.fr = tfidf;
                                                file_data.write((char*) &fr, sizeof(Fr));
                                        }

                                        frs.clear();
                                        last_word_id = current_word_id;
                                }
                                frs.push_back(Fr{aux.arr[j].doc_id, aux.arr[j].fr});
                        }
                }
                for(const auto& c : all_words[last_word_id]) {if(c == '\0') break; file_index<<c;}
                file_index<<','<<static_cast<std::uint32_t>(file_data.tellg())<<std::endl;

                std::uint32_t vec_sz = static_cast<std::uint32_t>(frs.size());

                std::uint32_t total_fr = 0;
                for(auto& fr : frs) total_fr += fr.fr;

                const std::uint32_t idf = static_cast<std::uint32_t>(1000*log10(static_cast<double>(1 + (total_fr/vec_sz))));
                file_data.write((char*) &idf, sizeof(std::uint32_t));

                file_data.write((char*) &vec_sz, sizeof(std::uint32_t));
                for(auto& fr : frs) {
                        const double tf = log10(static_cast<double>(1+fr.fr));
                        const std::uint32_t tfidf = static_cast<std::uint32_t>(tf*static_cast<double>(idf));
                        fr.fr = tfidf;
                        file_data.write((char*) &fr, sizeof(Fr));
                }
        }

        //Cerrar archivos restantes
        {
                file_index.close();
                file_data.close();
                file_orderedwordsfr.close();
        }
}