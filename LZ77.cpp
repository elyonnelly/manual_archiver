//
// Created by elyon on 01.04.2020.
//

#include "LZ77.h"

LZ77::LZ77(int buffer_size, int preview_size)
{
    this->buffer_size = buffer_size;
    this->preview_size = preview_size;
}

Code LZ77::getPrefix(std::string &s, int start_position, int current_buffer)
{
    int i = start_position - current_buffer; // начало поиска в словаре
    int j = start_position; // поиск в буфере предпросмотра
    int offset = 0, length = 0;
    char new_char = s[start_position];
    for (i = i; i < start_position; i++)
    {
        int i1 = i, j1 = j;
        while(s[i1] == s[j1] && j1 < s.length() && i1 < s.length())
        {
            i1++;
            j1++;
        }
        if (j1 - j > length)
        {
            length = j1 - j;
            offset = start_position - i;
            new_char = s[j1];
        }
    }
    return {offset, length, new_char};
}

std::vector<Code> LZ77::get_code(std::string &s)
{
    std::vector<Code> codes(0);
    int position = 0;
    int current_buffer = 0;
    while(position < s.length())
    {
        //нашли наибольший префикс строки в буфере предпросмотра, который есть в буфере предыстории
        Code c = getPrefix(s, position, current_buffer);
        int offset = c.offset; //позиция префикса в буфере предыстории
        int length = c.length; //длина префикса
        char next_char = c.next_char; //следующий за префиксом символ в буфере предпросмотра
        position += (length + 1); //сдвинулись на l + 1 в нашем окне предпросмотра
        current_buffer = std::min(current_buffer + (length + 1), buffer_size);
        codes.push_back(c);
    }

    return codes;
}
