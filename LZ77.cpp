//
// Created by elyon on 01.04.2020.
//

#include <fstream>
#include "LZ77.h"

using namespace std;

LZ77::LZ77(int dictionary_size, int window_size)
{
    this->dictionary_size = dictionary_size ;
    this->window_size = window_size ;
    this->buffer_size = this->dictionary_size + this->window_size;
}


void LZ77::encode(std::string input, std::string output)
{
    std::ifstream fin;

    fin.open(input, std::ios::binary);

    vector<unsigned char> text(window_size + dictionary_size); //кольцевой буффер такой

    int index = 0, size_of_file = 0;

    unsigned char ch;
    //сначала заполним буфер предпросмотра
    while(index < window_size && fin.read(reinterpret_cast<char *>(&ch), sizeof(char)))
    {
        text[index++] = ch;
        size_of_file++;
    }

    std::vector<Code> codes(0);

    int current_size_of_dict = 0;
    int start_of_window = 0;
    while (start_of_window < size_of_file)
    {
        Code c = getPrefix(text, start_of_window % buffer_size, current_size_of_dict);
        codes.push_back(c);

        start_of_window += (c.length + 1);
        current_size_of_dict = min(start_of_window, dictionary_size);

        //теперь нужно считать c.length + 1 символ и сдвинуть указатели на окно и словарь
        int cnt = 0;
        while (cnt < c.length + 1 && fin.read(reinterpret_cast<char *>(&ch), sizeof(char)))
        {
            text[index] = ch;
            index = (index + 1) % buffer_size;
            cnt++;
            size_of_file++;
        }
    }

    ofstream fout;
    fout.open(output, std::ios::binary);
    fout << codes.size() << "\n";

    for (int i = 0; i < codes.size(); ++i)
    {
        fout << codes[i].offset << " " << codes[i].length << " " << codes[i].next_char << " ";
    }
}

//есть шанс, что это работает..........
Code LZ77::getPrefix(vector<unsigned char> &s, int start_of_window, int current_size_of_dict)
{
    int i = start_of_window - current_size_of_dict >= 0 ?
            start_of_window - current_size_of_dict :
            start_of_window - current_size_of_dict + buffer_size; // начало поиска в словаре
    int j = start_of_window; // поиск в буфере предпросмотра

    //вот тут внести правки, до каких пор идти
    int offset = 0, length = 0;
    char new_char = s[start_of_window];

    //мы идем от start_of_dict до start_of_window, то есть длину словаря проходим просто
    for (int k = 0; k < current_size_of_dict; k++)
    {
        int i1 = (i + k) % buffer_size, j1 = j;

        //сколько раз можно выполнять эти действия? Пока до конца буфера предпросмотра не дойдем
        int shift = 0;
        while(shift < window_size && s[i1] == s[j1])
        {
            i1 = (i1 + 1) % buffer_size;
            j1 = (j1 + 1) % buffer_size;
            shift++;
        }

        if (shift > length)
        {
            length = shift;
            offset = current_size_of_dict - k;
            // а если у нас shift == window_size? и там произошел конец файла в общем-то.
            // нужно это как-то отслеживать..... а, ну по возвращаемому коду можно понимать.
            // вернее, вообще говоря,
            // при раскодировании всегда нужно будет просто игнорировать последний символ.
            new_char = s[j1];
        }
    }
    return {offset, length, new_char};
}

//у нас здесь уже как бы было считано всё окно
std::vector<Code> LZ77::get_code(vector<unsigned char> &s)
{
    std::vector<Code> codes(0);
    int position = 0;
    int current_dictionary_size = 0;
    while(position < s.size())
    {
        //нашли наибольший префикс строки в буфере предпросмотра, который есть в буфере предыстории
        Code c = getPrefix(s, position, current_dictionary_size);
        int offset = c.offset; //позиция префикса в буфере предыстории
        int length = c.length; //длина префикса
        char next_char = c.next_char; //следующий за префиксом символ в буфере предпросмотра
        position += (length + 1); //сдвинулись на l + 1 в нашем окне предпросмотра
        current_dictionary_size = std::min(current_dictionary_size + (length + 1), dictionary_size);
        codes.push_back(c);
    }

    return codes;
}

