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
    LZ77(int buffer_size, int preview_size);
    Code getPrefix(std::string &s, int start_position, int current_buffer);
    std::vector<Code> get_code(std::string &s);

private:
    int buffer_size;
    int preview_size;
};


#endif //ARCHIVER_FEDOROVA_ALENA_BSE185_LZ77_H
