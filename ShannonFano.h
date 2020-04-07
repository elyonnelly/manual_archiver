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
    /*
     * возвращает коэффициент сжатия
     */
    double encode(std::string input, std::string output);
    void decode(std::string input, std::string output);


private:
    vector<int> prefix_sum;
    vector<vector<int> > codes;
    vector<pair<int, int>> frequency;

    int get_median(int left, int right);
    void build();
    void build(int l, int r);
    string get (int i);
    void fill_prefix_sum();

    int count_frequencies_encode(string &input);
    void count_frequencies_decode(ifstream &fin);

};




#endif //ARCHIVER_FEDOROVA_ALENA_BSE185_SHANNONFANO_H
