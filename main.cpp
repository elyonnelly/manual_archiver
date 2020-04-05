// КДЗ по дисциплине Алгоритмы и структуры данных, 2019-2020 уч.год
// Федорова Алена Валерьевна, группа БПИ-185, дата (07.04.2020)
// Clion,
// Состав проекта (файлы *.cpp и *.h)
// Что сделано:
// Сжатие и распаковка методом Шеннона - Фано;
// Сжатие и распаковка методом LZ77;
// Сжатие и распаковка методом LZW(*);
// проведен вычислительный эксперимент;
// построены таблицы и графики;
//
// для измерения времени выполнения использовалось ХХХ;
// оформлен отчет)

// Что не сделано:

#include <iostream>
#include <future>
#include <chrono>
#include <fstream>
#include <map>
#include <cmath>
#include "ShannonFano.h"
#include "LZ77.h"

using namespace std;

/*
 * время упаковки с помощью LZ77, размером словаря dictionary_size и размером окна window_size
 * и коэффициент сжатия
 */
pair<double, double> time_measurement_encode_LZ77(LZ77 &lz, int dictionary_size, int window_size,
                            string input, string encode)
{
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();
    double compression_ratio = lz.encode(input, encode);
    end = chrono::system_clock::now();
    double elapsed_seconds = (chrono::duration_cast<chrono::milliseconds>
            (end-start).count()) * 0.001;
    return {elapsed_seconds, compression_ratio};
}

/*
 * время распаковки с помощью LZ77, размером словаря dictionary_size и размером окна window_size
 */
double time_measurement_decode_LZ77(LZ77 &lz, int dictionary_size, int window_size,
                                                  string encode, string decode)
{
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();
    lz.decode(encode, decode);
    end = chrono::system_clock::now();
    double elapsed_seconds = (chrono::duration_cast<chrono::milliseconds>
            (end-start).count()) * 0.001;
    return elapsed_seconds;
}

/*
 * время упаковки с помощью ShannonFano и коэффициент сжатия
 */
pair<double, double> time_measurement_encode_ShannonFano(ShannonFano &sh, string input, string encode)
{
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();
    double compression_ratio  = sh.encode(input, encode);
    end = chrono::system_clock::now();
    double elapsed_seconds = (chrono::duration_cast<chrono::milliseconds>
            (end-start).count()) * 0.001;
    return {elapsed_seconds, compression_ratio};
}

/*
 * время распаковки с помощью ShannonFano
 */
double time_measurement_decode_ShannonFano(ShannonFano &sh, string encode, string decode)
{
    chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();
    sh.decode(encode, decode);
    end = chrono::system_clock::now();
    double elapsed_seconds = (chrono::duration_cast<chrono::milliseconds>
            (end-start).count()) * 0.001;
    return elapsed_seconds;
}

double calc_entropy(string file)
{
    ifstream fin;
    fin.open(file, ios::binary);
    unsigned char ch;
    map<unsigned char, int> frequency;
    while(fin.read(reinterpret_cast<char *>(&ch), sizeof(unsigned char)))
    {
        frequency[ch]++;
    }
    int different_ch = frequency.size();
    double H = 0;
    for (auto w : frequency)
    {
        double w_i = w.second/(double)different_ch;
        H += w_i * (log(w_i)/log(2));
    }
    H *= -1;
    return H;
}

int main()
{

    
    return 0;
}