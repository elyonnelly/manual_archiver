//
// Created by elyon on 01.04.2020.
//

#include <fstream>
#include "LZ77.h"

using namespace std;

LZ77::LZ77(int window_size, int dictionary_size)
{
    this->dictionary_size = dictionary_size * 1024;
    this->window_size = window_size * 1024;
    this->buffer_size = this->dictionary_size + this->window_size;
}

double LZ77::encode(std::string input, std::string output)
{
    auto res = get_codes(input);
    ofstream fout;
    fout.open(output, std::ios::binary);
    fout.write(reinterpret_cast<const char *>(&res.second), sizeof(int));

    for (int i = 0; i < res.first.size(); ++i)
    {
        fout.write(reinterpret_cast<const char *>(&res.first[i].offset), sizeof(short));
        fout.write(reinterpret_cast<const char *>(&res.first[i].length), sizeof(short));
        fout.write(reinterpret_cast<const char *>(&res.first[i].next_char), sizeof(char));
    }
    return (res.first.size() * (sizeof(short) * 2 + sizeof(char)))/(double)res.second;
}

Code LZ77::get_prefix(vector<unsigned char> &s, int start_of_window, int current_size_of_dict)
{
    int i = start_of_window - current_size_of_dict >= 0 ?
            start_of_window - current_size_of_dict :
            start_of_window - current_size_of_dict + buffer_size; // начало поиска в словаре
    int j = start_of_window; // поиск в буфере предпросмотра

    //вот тут внести правки, до каких пор идти
    int offset = 0, length = 0;
    unsigned char new_char = s[start_of_window];

    //мы идем от start_of_dict до start_of_window, то есть длину словаря проходим просто
    for (int k = 0; k < current_size_of_dict; k++)
    {
        int i1 = (i + k) % buffer_size, j1 = j;

        //сколько раз можно выполнять эти действия? Пока до конца буфера предпросмотра не дойдем
        int shift = 0;
        while (shift < window_size && s[i1] == s[j1])
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
    return {(short)offset, (short)length, new_char};
}

std::pair<vector<Code>, int> LZ77::get_codes(string input)
{
    std::ifstream fin;

    fin.open(input, std::ios::binary);

    vector<unsigned char> text(window_size + dictionary_size); //кольцевой буффер

    int index = 0, size_of_file = 0;

    unsigned char ch;
    //заполним буфер предпросмотра
    while (fin.is_open() && index < window_size && fin.read(reinterpret_cast<char *>(&ch), sizeof(char)))
    {
        text[index++] = ch;
        size_of_file++;
    }

    std::vector<Code> codes(0);

    int current_size_of_dict = 0;
    int start_of_window = 0;
    while (start_of_window < size_of_file)
    {

        Code c = get_prefix(text, start_of_window % buffer_size, current_size_of_dict);
        codes.push_back(c);

        start_of_window += c.length + 1;
        current_size_of_dict = min(start_of_window, dictionary_size);

        //теперь нужно считать c.length + 1 символ и сдвинуть указатели на окно и словарь
        int cnt = 0;
        while (fin.is_open() && cnt < c.length + 1 && fin.read(reinterpret_cast<char *>(&ch), sizeof(char)))
        {
            text[index] = ch;
            index = (index + 1) % buffer_size;
            cnt++;
            size_of_file++;
        }
    }

    fin.close();

    return {codes, size_of_file};
}

void LZ77::decode(std::string input, std::string output)
{
    vector<Code> codes(0);
    short offset, length;
    unsigned char new_char;

    ifstream fin;
    fin.open(input, ios::binary);
    int size_of_file;
    fin.read(reinterpret_cast<char *>(&size_of_file), sizeof(int));

    while(fin.read(reinterpret_cast<char *>(&offset), sizeof(short))
            && fin.read(reinterpret_cast<char *>(&length), sizeof(short))
            && fin.read(reinterpret_cast<char *>(&new_char), sizeof(char)))
    {
        codes.push_back({offset, length, new_char});
    }

    vector<unsigned char > text(0);
    text.push_back(codes[0].next_char);
    int position = 1;
    for (int i = 1; i < codes.size(); ++i)
    {
        for (int j = position - codes[i].offset; j < min(position - codes[i].offset + codes[i].length, (int)text.size()); ++j)
        {
            text.push_back(text[j]);
        }
        if (text.size() < size_of_file)
        {
            text.push_back(codes[i].next_char);
        }

        position = text.size();
    }

    ofstream fout;
    fout.open(output, ios::binary);
    for (int i = 0; i < text.size(); ++i)
    {
        fout.write(reinterpret_cast<const char *>(&text[i]), sizeof(char));
    }

}

