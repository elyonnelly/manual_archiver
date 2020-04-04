//
// Created by elyon on 01.04.2020.
//

#ifndef ARCHIVER_FEDOROVA_ALENA_BSE185_LZ77_H
#define ARCHIVER_FEDOROVA_ALENA_BSE185_LZ77_H

#include <iostream>
#include <vector>

struct Code
{
    int offset, length;
    char next_char;
};

class LZ77
{
public:
    LZ77(int dictionary_size, int window_size);
    void encode(std::string input, std::string output);
    void decode(std::string input, std::string output);

private:
    Code getPrefix(std::vector<unsigned char> &s, int start_of_window, int current_dictionary_size);
    std::vector<Code> get_code(std::vector<unsigned char> &s);

private:
    int dictionary_size;
    int window_size;
    int buffer_size;
};


#endif //ARCHIVER_FEDOROVA_ALENA_BSE185_LZ77_H
