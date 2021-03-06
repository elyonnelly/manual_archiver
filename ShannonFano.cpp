//
// Created by elyon on 01.04.2020.
//

#include "ShannonFano.h"
#include <map>
#include <unordered_map>
#include <fstream>
#include <algorithm>

using namespace std;


ShannonFano::ShannonFano()
{

}

void ShannonFano::decode(std::string input, std::string output)
{
    frequency.resize(0);
    codes.resize(0);
    prefix_sum.resize(0);

    std::ifstream fin;
    fin.open(input, std::ios::binary);

    count_frequencies_decode(fin);
    int number_of_bytes;
    fin.read(reinterpret_cast<char *>(&number_of_bytes), sizeof(int));
    build();
    unordered_map<string, unsigned char> codes_byte;
    for (int i = 0; i < frequency.size(); ++i)
    {
        codes_byte[get(i)] = frequency[i].second;
    }

    std::ofstream fout;
    fout.open(output, std::ios::binary);

    if (fin.is_open() && fout.is_open())
    {
        int count = 0;
        char byte;
        storage temp;
        string str_code;
        while (fin.read(&byte, sizeof(byte)) && count != number_of_bytes)
        {
            temp.ch = byte;
            for(int i = 7; i >= 0; i--)
            {
                str_code += '0' + temp.byte[i];
                if (count < number_of_bytes && codes_byte.find(str_code) != codes_byte.end())
                {
                    char symbol = (char)codes_byte[str_code];
                    fout.write(&symbol, sizeof(char));
                    str_code = "";
                    count++;
                }
            }
        }
    }
    fin.close();
    fout.close();
}


double ShannonFano::encode(std::string input, std::string output)
{
    frequency.resize(0);
    codes.resize(0);
    prefix_sum.resize(0);
    //размер исходного файла
    int original_size = count_frequencies_encode(input);
    build();

    //получили коды всех встречающихся символов
    vector<int>* codes_byte[256];
    int frequency_byte[256];
    fill(frequency_byte, frequency_byte + 256, 0);
    for (int i = 0; i < frequency.size(); ++i)
    {
        codes_byte[frequency[i].second] = &codes[i];
        frequency_byte[frequency[i].second] = frequency[i].first;
    }

    std::ifstream fin;
    std::ofstream fout;

    fout.open(output, std::ios::binary);
    //размер архивированного файла
    int encode_size = 0;
    //здесь нужно записать табличку частот в начало файла. байт-частота-байт-частота...
    for (int i = 0; i < 256; ++i)
    {
        char byte = i;
        fout.write(&byte, sizeof(char));
        fout.write(reinterpret_cast<const char *>(&frequency_byte[i]), sizeof(int));
    }
    encode_size += 256 * (sizeof(char) + sizeof(int));
    
    //запишем количество байтов в исходном файле
    fout.write(reinterpret_cast<const char *>(&original_size), sizeof(int));
    encode_size += sizeof(int);

    fin.open(input, std::ios::binary); //какое отличие от мода по умолчанию?
    if (fin.is_open())
    {
        char byte;
        storage temp;
        temp.ch = 0;
        int i = 7;
        while(fin.read(&byte, sizeof(char)))
        {
            //у этого символа -- byte -- код codes_byte[byte]
            unsigned char a = byte;
            auto code_string = codes_byte[a];
            for(auto b : *code_string)
            {
                temp.byte.set(i--, b);
                if(i == -1)
                {
                    i = 7;
                    fout.write(reinterpret_cast<const char *>(&temp.ch), sizeof(char));
                    encode_size += sizeof(char);
                    temp.ch = 0;
                }
            }
        }
        if (i != 7)
        {
            char a = temp.ch;
            fout.write(&a, sizeof(char));
            encode_size += sizeof(char);
        }
        temp.ch = 0;
    }
    
    fin.close();
    fout.close();

    return encode_size/(double)original_size;
}

int ShannonFano::count_frequencies_encode(std::string &input)
{
    std::ifstream fin;
    fin.open(input, std::ios::binary);

    int temp[256];
    fill(temp, temp + 256, 0);
    int original_size = 0;
    if (fin.is_open())
    {
        unsigned char byte;
        while(fin.read(reinterpret_cast<char *>(&byte), sizeof(byte)))
        {
            temp[byte]++;
            original_size++;
        }
    }

    for (int i = 0; i < 256; ++i)
    {
        if (temp[i] > 0)
        {
            frequency.emplace_back(temp[i], i); //по сути i -- это байт
            //если и делать мапу, то здесь
        }
    }
    std::sort(frequency.rbegin(), frequency.rend()); //проверить, лучше ли так или своим пузырьком

    fin.close();//опробовать два варианта: "закрывать и заново открывать" либо "не закрывать"

    //cout << count;
    return original_size;
}


void ShannonFano::build()
{
    fill_prefix_sum();
    for (int i = 0; i < frequency.size(); ++i)
    {
        codes.emplace_back(0);
    }
    build(0, frequency.size() - 1);
}

void ShannonFano::build(int l, int r)
{
    if (l >= r)
        return;
    int m = get_median(l + 1, r + 1);
    for (int i = l; i < m; i++)
    {
        codes[i].push_back(0);
    }
    for (int i = m; i <= r; i++)
    {
        codes[i].push_back(1);
    }
    build(l, m-1);
    build(m, r);
}

std::string ShannonFano::get(int i)
{
    // выдает битовый код i символа
    std::string s;
    for (int j : codes[i])
        s += j + '0';
    return s;
}

int ShannonFano::get_median(int left, int right)
{
    int sum = (prefix_sum[right] - prefix_sum[left - 1]);
    int m = 0;
    for (int i = left; i <= right; i++)
    {
        //i включено в эту левую половину
        int delta = prefix_sum[i] - prefix_sum[left - 1];
        if (delta >= sum / 2)
        {
            m = i;
            break;
        }
    }
    int delta1 = prefix_sum[m] - prefix_sum[left - 1];
    int delta2 = prefix_sum[m - 1] - prefix_sum[left - 1];
    if (abs(sum - delta1 - delta1) < abs(sum - delta2 - delta2))
    {
        return m;
    }
    else
    {
        return m - 1;
    }
}

void ShannonFano::fill_prefix_sum()
{
    prefix_sum.resize(frequency.size() + 1);
    prefix_sum[0] = 0;
    for (int i = 0; i < frequency.size(); ++i)
    {
        prefix_sum[i + 1] = prefix_sum[i] + frequency[i].first;
    }
}


void ShannonFano::count_frequencies_decode(ifstream &fin)
{
    if (fin.is_open())
    {
        for (int i = 0; i < 256; ++i)
        {
            unsigned char byte;
            fin.read(reinterpret_cast<char *>(&byte), sizeof(char));
            int freq;
            fin.read(reinterpret_cast<char *>(&freq), sizeof(int));

            if (freq > 0)
            {
                frequency.emplace_back(freq, byte);
            }
        }
    }
    sort(frequency.rbegin(), frequency.rend());
}

