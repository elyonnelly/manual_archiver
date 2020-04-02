//
// Created by elyon on 01.04.2020.
//

#ifndef ARCHIVER_FEDOROVA_ALENA_BSE185_SHANNONFANO_H
#define ARCHIVER_FEDOROVA_ALENA_BSE185_SHANNONFANO_H


#include <vector>
#include <iostream>
#include <bitset>

using namespace std;

class ShannonFano
{
    union storage
    {
        unsigned char ch;
        bitset<8> byte;

        storage()
        {

        }
    };


public:

    ShannonFano();
    void encode(std::string input, std::string output);
    void decode(std::string input, std::string output);


private:
    vector<int> prefix_sum;
    vector<vector<int> > code;
    vector<pair<int, int>> frequency;

    //Возвращает медиану m. Тогда мы делим массив на части как [l;m-1] и [m;r]
    int get_median(int left, int right);
    void build();
    void build(int l, int r);
    string get (int i);
    void fill_prefix_sum();

    int count_frequencies_encode(string &input);
    void count_frequencies_decode(ifstream &fin);

};




#endif //ARCHIVER_FEDOROVA_ALENA_BSE185_SHANNONFANO_H
