#include <array>
#include <fstream>
#include <cstdint>
#include <iostream>
#include <assert.h>
#include <cmath>
#include <string>


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


void order(std::fstream& file_wordsfr, std::fstream& file_orderedwordsfr, const std::uint32_t& n_blocks, const std::uint32_t& blockset_sz){

        const std::uint32_t r_blocks = n_blocks%blockset_sz;
        const std::uint32_t n_blocksets = (r_blocks == 0) ? n_blocks/blockset_sz : (n_blocks/blockset_sz) + 1;
        const std::uint32_t double_blocksets = (n_blocksets % 2 == 0) ? n_blocksets : n_blocksets - 1;


        Block output;
        Block input_a;
        Block input_b;
        std::uint32_t blockset_sz_b = blockset_sz;
        
        std::cout<<"n_blocks: "<<n_blocks<<std::endl;
        std::cout<<"blockset_sz: "<<blockset_sz<<std::endl;
        std::cout<<"r_blocks: "<<r_blocks<<std::endl;
        std::cout<<"n_blocksets: "<<n_blocksets<<std::endl;
        std::cout<<"double_blocksets: "<<double_blocksets<<std::endl<<std::endl;

        for(std::uint32_t i = 0; i<double_blocksets; i+=2){
                std::uint32_t blockset_count_a = 0; // iterar entre blocks
                std::uint32_t blockset_count_b = 0;

                std::uint32_t block_count_a = 0; // iterar entre los elementos del block
                std::uint32_t block_count_b = 0;

                std::uint32_t output_count = 0;

                file_wordsfr.seekg(i*blockset_sz*sizeof(Block), std::ios::beg);
                file_wordsfr.read((char*) &input_a, sizeof(Block));
                
                file_wordsfr.seekg((i+1)*blockset_sz*sizeof(Block), std::ios::beg);
                file_wordsfr.read((char*) &input_b, sizeof(Block));

                if (i == double_blocksets-2) if (r_blocks != 0) blockset_sz_b = r_blocks;

                while(true){
                        //si se llena el block de output
                        if(output.current_size == Block_sz){
                                file_orderedwordsfr.write((char*) &output, sizeof(Block));
                                output_count++;
                                output.current_size = 0;
                        }

                        //si se termina de iterar por el block a
                        if(block_count_a == input_a.current_size){
                                blockset_count_a++;

                                //si ya no hay más blocks en el grupo de blocks de a
                                if (blockset_count_a == blockset_sz) {

                                        //mientras haya blocks en el grupo de blocks de b
                                        while (blockset_count_b < blockset_sz_b) {

                                                //mientras haya más por leer en un block b 
                                                while (block_count_b < input_b.current_size) {
                                                        
                                                        //si se llena el block de output
                                                        if(output.current_size == Block_sz){
                                                                file_orderedwordsfr.write((char*) &output, sizeof(Block));
                                                                output_count++;
                                                                output.current_size = 0;
                                                        }
                                                        output.arr[output.current_size++] = input_b.arr[block_count_b++];
                                                }

                                                //si ya no hay más por leer en un block b
                                                blockset_count_b++;
                                                if(blockset_count_b == blockset_sz_b) break;
                                                file_wordsfr.seekg((i+1)*blockset_sz*sizeof(Block) + blockset_count_b*sizeof(Block), std::ios::beg);
                                                file_wordsfr.read((char*) &input_b, sizeof(Block));
                                                block_count_b = 0;
                                        }
                                        
                                        //si ya no hay más blocks en el grupo de blocks de b
                                        break;

                                }else{
                                        //si hay más blocks en el grupo de blocks de a 
                                        file_wordsfr.seekg(i*blockset_sz*sizeof(Block) + blockset_count_a*sizeof(Block), std::ios::beg);
                                        file_wordsfr.read((char*) &input_a, sizeof(Block));
                                }

                                block_count_a = 0;
                                continue;
                        }

                        //si se termina de iterar por b
                        if(block_count_b == input_b.current_size){
                                blockset_count_b++;

                                //si ya no hay más blocks en el grupo de blocks de b
                                if (blockset_count_b == blockset_sz_b) {

                                        //mientras haya blocks en el grupo de blocks de a
                                        while (blockset_count_a < blockset_sz) {

                                                //mientras haya más por leer en el block a
                                                while (block_count_a < input_a.current_size) {

                                                        //si se llena el block de output
                                                        if(output.current_size == Block_sz){
                                                                file_orderedwordsfr.write((char*) &output, sizeof(Block));
                                                                output_count++;
                                                                output.current_size = 0;
                                                        }
                                                        output.arr[output.current_size++] = input_a.arr[block_count_a++];
                                                }

                                                //si ya no hay más por leer en el block a
                                                blockset_count_a++;
                                                if(blockset_count_a == blockset_sz) break;
                                                file_wordsfr.seekg(i*blockset_sz*sizeof(Block) + blockset_count_a*sizeof(Block), std::ios::beg);
                                                file_wordsfr.read((char*) &input_a, sizeof(Block));
                                                block_count_a = 0;
                                        }

                                        //si ya no hay blocks en el grupo de blocks de a
                                        break;
                                
                                }else{
                                        //si hay más blocks en el grupo de blocks de b
                                        file_wordsfr.seekg((i+1)*blockset_sz*sizeof(Block) + blockset_count_b*sizeof(Block), std::ios::beg);
                                        file_wordsfr.read((char*) &input_b, sizeof(Block));
                                }

                                block_count_b = 0;
                                continue;
                        }

                        //si el elemento del block a es menor al elemento del block b
                        if(input_a.arr[block_count_a].word_id < input_b.arr[block_count_b].word_id){
                                output.arr[output.current_size++] = input_a.arr[block_count_a++];

                        }else{
                                //si el elemento del block a no es menor al elemento del block b 
                                output.arr[output.current_size++] = input_b.arr[block_count_b++];
                        }
                        
                }

                //si el block output tienen un tamaño mayor a 0
                if (output.current_size > 0) {
                        file_orderedwordsfr.write((char*) &output, sizeof(Block));
                        output_count++;
                        output.current_size = 0;
                }
                
                //mientras la cantidad de blocks de output sea menor al doble del blockset_sz
                while(output_count < blockset_sz*2){
                        output.current_size = 0;
                        file_orderedwordsfr.write((char*) &output, sizeof(Block));
                        output_count++;
                }
        }

        if (n_blocksets % 2 != 0) {
                const std::uint32_t last_blockset_sz = (r_blocks == 0) ? blockset_sz : r_blocks;
                for (std::uint32_t blockset_count = 0; blockset_count < last_blockset_sz; blockset_count++) {
                        file_wordsfr.seekg((n_blocksets-1)*blockset_sz*sizeof(Block) + blockset_count*sizeof(Block), std::ios::beg);
                        file_wordsfr.read((char*) &output, sizeof(Block));
                        file_orderedwordsfr.write((char*) &output, sizeof(Block));
                }
        }
}

void copy_file(std::fstream &file_input, std::fstream &file_orderedwordsfr, std::uint32_t n_blocks){
        Block aux;
        file_input.seekg(0, std::ios::beg);
        for(std::uint32_t i = 0; i < n_blocks; ++i){
                file_input.read((char*) &aux, sizeof(Block));
                file_orderedwordsfr.write((char*) &aux, sizeof(Block));
        }
}

int main(void){
        
        //Abrir archivos
        std::fstream file_wordsfr;
        std::fstream file_orderedwordsfr;
        {
                file_wordsfr.open("INPUT/WordsFr/wordsfr.dat", std::fstream::in | std::fstream::binary);
                assert(file_wordsfr.is_open());

                file_orderedwordsfr.open("OrderedWordsFr/orderedwordsfr.dat", std::fstream::out | std::fstream::trunc | std::fstream::binary);
                assert(file_orderedwordsfr.is_open());
        }

        
        //Contar la cantidad de blocks
        std::uint32_t n_blocks;
        {
                file_wordsfr.seekg(0, std::ios::end);
                n_blocks = (static_cast<std::uint32_t>(file_wordsfr.tellg())+1)/sizeof(Block);
        }

        //Ordenar
        {
                const auto n_iterations = static_cast<std::uint32_t>(ceil(log2(n_blocks)));
                std::string path = "OrderedWordsFr/orderedwordsfr";
                std::string _dat = ".dat";

                std::fstream file_input;
                std::fstream file_output;

                std::uint32_t blockset_sz = 1;
                for( std::uint32_t i = 0; i < n_iterations; ++i){
                    if(i == 0){
                            file_output.open(path+std::to_string(1)+_dat, std::fstream::out | std::fstream::trunc | std::fstream::binary);
                            assert(file_output.is_open());

                            order(file_wordsfr, file_output, n_blocks, blockset_sz);

                            file_output.close();
                    }else{
                            file_input.open(path+std::to_string(i)+_dat, std::fstream::in | std::fstream::binary);
                            assert(file_input.is_open());

                            file_output.open(path+std::to_string(i+1)+_dat, std::fstream::out | std::fstream::trunc | std::fstream::binary);
                            assert(file_output.is_open());

                            
                            file_input.seekg(0, std::ios::end);
                            n_blocks = (static_cast<std::uint32_t>(file_input.tellg())+1)/sizeof(Block);

                            blockset_sz = blockset_sz * 2;
                            order(file_input, file_output, n_blocks, blockset_sz);

                            file_input.close();
                            file_output.close();
                    }
                }
                file_input.open(path+std::to_string(n_iterations)+_dat, std::fstream::in | std::fstream::binary);
                file_input.seekg(0, std::ios::end);
                n_blocks = (static_cast<std::uint32_t>(file_input.tellg())+1)/sizeof(Block);
                copy_file(file_input, file_orderedwordsfr, n_blocks);
                file_input.close();
                
        }

        //Cerrar archivos
        {
                file_wordsfr.close();
                file_orderedwordsfr.close();
        }
}