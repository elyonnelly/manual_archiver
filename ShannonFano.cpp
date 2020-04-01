//
// Created by elyon on 01.04.2020.
//

#include "ShannonFano.h"
#include <map>
#include <fstream>
#include <algorithm>

using namespace std;


void ShannonFano::decode(std::string input, std::string output)
{
    std::ifstream fin;
    fin.open(input, std::ios::binary);
    if (fin.is_open())
    {
        char byte;
        storage temp;
        int i = 0;
        while (!fin.eof())
        {
            fin.read(&byte, sizeof(byte));
            if (!fin.eof())
            {
                temp.ch = byte;
                cout << temp.byte;
            }
        }
    }
}


void ShannonFano::encode(std::string input, std::string output)
{
    count_frequencies(input);
    std::sort(frequency.begin(), frequency.end()); //проверить, лучше ли так или своим пузырьком
    build();

    //получили коды всех встречающихся символов
    string codes[256];
    for (int i = 0; i < frequency.size(); ++i)
    {
        codes[frequency[i].second] = get(i);
    }

    std::ifstream fin;
    std::ofstream fout;
    fin.open(input, std::ios::binary); //какое отличие от мода по умолчанию?
    fout.open(output, std::ios::binary);
    if (fin.is_open())
    {
        char byte;
        storage temp;
        temp.ch = 0;
        int i = 7;
        while(!fin.eof())
        {
            fin.read(&byte, sizeof(byte));
            if (!fin.eof())
            {
                //у этого символа -- byte -- код codes[byte]
                auto code_string = codes[byte];
                //cout << code_string << " ";
                for(auto b : code_string)
                {
                    temp.byte.set(i--, b == '1');
                    if(i == -1)
                    {
                        i = 7;
                        char a = temp.ch;
                        fout.write(&a, sizeof(char));
                        cout << temp.byte;

                        temp.ch = 0;
                    }
                }
                //cout << temp.byte << "\n";

            }
        }
        char a = temp.ch;
        fout.write(&a, sizeof(char));
        cout << temp.byte;

        //cout << temp.byte;
        temp.ch = 0;
    }

    fin.close();
    fout.close();
}

void ShannonFano::count_frequencies(std::string &input)
{
    std::ifstream fin;
    fin.open(input, std::ios::binary);

    int temp[256];
    fill(temp, temp + 256, 0);
    if (fin.is_open())
    {
        char byte;
        while(!fin.eof())
        {
            fin.read(&byte, sizeof(byte));
            if (!fin.eof())
            {
                temp[byte]++;
            }
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

    fin.close();//опробовать два варианта: "закрывать и заново открывать" либо "не закрывать"
}


void ShannonFano::build()
{
    fill_prefix_sum();
    code.resize(frequency.size());
    build(0, frequency.size() - 1);
}

void ShannonFano::build(int l, int r)
{
    if (l >= r)
        return;
    int m = get_median(l + 1, r + 1);
    for (int i = l; i < m; i++)
    {
        code[i].push_back(0);
    }
    for (int i = m; i <= r; i++)
    {
        code[i].push_back(1);
    }
    build(l, m-1);
    build(m, r);
}

std::string ShannonFano::get(int i)
{
    // выдает битовый код i символа
    std::string s;
    for (int j : code[i])
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
    for (int i = 0; i < prefix_sum.size(); ++i)
    {
        prefix_sum[i + 1] = prefix_sum[i] + frequency[i].first;
    }
}

