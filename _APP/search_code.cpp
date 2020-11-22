#include <array>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <assert.h>
#include <string>
#include <vector>
#include <sstream>
#include <map>
#include <algorithm>
#include <cmath>
#include <chrono>



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

struct Distance{
        std::uint32_t doc_id;
        double dist;
};

double cosine_similarity(const std::vector<double>& A, const std::vector<double>& B, const std::uint32_t& ft_sz){
    double dot = 0.0, denom_a = 0.0, denom_b = 0.0 ;
     for(unsigned int i = 0u; i < ft_sz; ++i) {
        dot += A[i] * B[i] ;
        denom_a += A[i] * A[i] ;
        denom_b += B[i] * B[i] ;
    }
    return dot / (sqrt(denom_a) * sqrt(denom_b)) ;
}

int main(int argc, char** argv){

        //abrir archivos
        std::fstream file_results;
        std::fstream file_data;
        std::fstream file_names;
        {
            file_results.open("_results.txt" , std::fstream::out | std::fstream::trunc);
            assert(file_results.is_open());

            file_data.open("INPUT/InvertedIndex/data.dat" , std::fstream::in | std::fstream::binary);
            assert(file_data.is_open());

            file_names.open("INPUT/INPUT/INPUT/INPUT/INPUT/INPUT/RawDocs/Names/names.txt" , std::fstream::in);
            assert(file_names.is_open());

        }

        //cargar los features del query y los punteros a vectores
        std::vector<std::uint32_t> features_ptrs;
        std::vector<double> features_query;
        {
                std::uint32_t ptr;
                double fr;
                for(std::uint32_t i = 1; i<argc; i += 2){
                        std::stringstream ss_ptr(argv[i]);
                        std::stringstream ss_fr(argv[i+1]);
                        ss_ptr >> ptr;
                        ss_fr >> fr;
                        features_ptrs.push_back(ptr);
                        features_query.push_back(fr);
                }
        }

        const auto start = std::chrono::high_resolution_clock::now();
        //cargar los features de los documentos
        std::map<std::uint32_t, std::vector<double>> features_docs;
        {
                std::uint32_t idf;
                std::uint32_t vec_sz;
                const std::uint32_t ft_sz = features_query.size();
                for(std::uint32_t i = 0; i<ft_sz; ++i){
                        
                        file_data.seekg(features_ptrs[i], std::fstream::beg);


                        file_data.read((char*) &idf, sizeof(std::uint32_t));
                        features_query[i] = log10(1 + features_query[i]) * static_cast<double>(idf);


                        file_data.read((char*) &vec_sz, sizeof(std::uint32_t));
                        for(std::uint32_t j = 0; j<vec_sz; ++j) {
                                Fr fr; file_data.read((char*) &fr, sizeof(Fr));
                                auto it = features_docs.find(fr.doc_id);

                                if(it != features_docs.end()){
                                        it->second[i] = static_cast<double>(fr.fr);
                                }else{
                                        features_docs[fr.doc_id] = std::vector<double>(ft_sz, 0.0);
                                        features_docs[fr.doc_id][i] = static_cast<double>(fr.fr);
                                }
                        }
                }
        }

        //calcular distancias
        std::vector<Distance> distances;
        {
                for(const auto& features_doc : features_docs){
                        distances.push_back({features_doc.first, cosine_similarity(features_query, features_doc.second, features_query.size())});
                }
        }

        //ordenar las distancias
        std::sort(distances.begin(), distances.end(), [](const Distance& d1, const Distance& d2){return d1.dist>d2.dist;});

        const auto stop = std::chrono::high_resolution_clock::now();
        //mostrar los resultados
        {
                const auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start).count();
                file_results<<"DOCUMENTOS SIMILARES: "<<distances.size()<<"\nTIEMPO TOTAL: "<<duration<<"ms\n\n"<<std::endl;
                file_results<<"PATH: INPUT/INPUT/INPUT/INPUT/INPUT/INPUT/RawDocs/Docs/...\n\n"<<std::endl;
                for(const auto& d : distances){
                        std::string doc_name;
                        file_names.seekg(d.doc_id*11, std::fstream::beg);
                        std::getline(file_names, doc_name);
                        file_results << doc_name << std::endl;
                }
        }

        //cerrar archivos
        {
                file_results.close();
                file_data.close();
                file_names.close();
        }
}