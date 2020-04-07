// КДЗ по дисциплине Алгоритмы и структуры данных, 2019-2020 уч.год
// Федорова Алена Валерьевна, группа БПИ-185, дата (07.04.2020)
// Clion,

// Состав проекта (файлы *.cpp и *.h):
//main.cpp
//LZ77.cpp и LZ77.h
//ShannonFano.cpp и ShannonFano.h

// Что сделано:
// Сжатие и распаковка методом Шеннона - Фано;
// Сжатие и распаковка методом LZ77;
// проведен вычислительный эксперимент;
// построены таблицы и графики;
// для измерения времени выполнения использовалась библиотека std::chrono;
// оформлен отчет

// Что не сделано: LZW

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
pair<double, double> time_measurement_encode_LZ77(LZ77 &lz,
                            string input, string encode)
{
    double compression_ratio = 0;
    double elapsed_seconds = 0;
    chrono::time_point<chrono::system_clock> start, end;
    for (int i = 0; i < 13; ++i)
    {
        start = chrono::system_clock::now();
        compression_ratio = lz.encode(input, encode);
        end = chrono::system_clock::now();
        if (i > 2)
        {
            elapsed_seconds += (chrono::duration_cast<chrono::milliseconds>
                    (end-start).count());
        }
    }
    return {elapsed_seconds/10, compression_ratio};
}

/*
 * время распаковки с помощью LZ77, размером словаря dictionary_size и размером окна window_size
 */
double time_measurement_decode_LZ77(LZ77 &lz, string encode, string decode)
{
    chrono::time_point<chrono::system_clock> start, end;
    double elapsed_seconds = 0;
    for (int i = 0; i < 13; ++i)
    {
        start = chrono::system_clock::now();
        lz.decode(encode, decode);
        end = chrono::system_clock::now();
        if (i > 2)
        {
            elapsed_seconds += (chrono::duration_cast<chrono::milliseconds>
                    (end-start).count());
        }
    }
    return elapsed_seconds / 10;
}

/*
 * время упаковки с помощью ShannonFano и коэффициент сжатия
 */
pair<double, double> time_measurement_encode_ShannonFano(ShannonFano &sh, string input, string encode)
{
    double compression_ratio = 0;
    double elapsed_seconds = 0;
    chrono::time_point<chrono::system_clock> start, end;
    for (int i = 0; i < 13; ++i)
    {
        start = chrono::system_clock::now();
        compression_ratio = sh.encode(input, encode);
        end = chrono::system_clock::now();
        if (i > 2)
        {
            elapsed_seconds += (chrono::duration_cast<chrono::milliseconds>
                    (end-start).count());
        }
    }

    return {elapsed_seconds / 10, compression_ratio};
}

/*
 * время распаковки с помощью ShannonFano
 */
double time_measurement_decode_ShannonFano(ShannonFano &sh, string encode, string decode)
{
    chrono::time_point<chrono::system_clock> start, end;
    double elapsed_seconds = 0;
    for (int i = 0; i < 13; ++i)
    {
        start = chrono::system_clock::now();
        sh.decode(encode, decode);
        end = chrono::system_clock::now();
        if (i > 2)
        {
            elapsed_seconds += (chrono::duration_cast<chrono::milliseconds>
                    (end-start).count());
        }
    }
    return elapsed_seconds/10;
}


double calc_entropy(ofstream &fout, string file)
{
    ifstream fin;
    fin.open(file, ios::binary);
    unsigned char ch;
    map<unsigned char, int> frequency;
    int size_of_file = 0;
    while(fin.read(reinterpret_cast<char *>(&ch), sizeof(unsigned char)))
    {
        frequency[ch]++;
        size_of_file++;
    }
    long double H = 0;
    for (auto w : frequency)
    {
        long double w_i = w.second/(double)size_of_file;
        long double logw = log(w_i)/log(2);
        H += w_i * logw;
    }

    fout << "****" + file + "****\n";
    for (int i = 0; i < 256; ++i)
    {
        fout << frequency[i]/(double)size_of_file << "\n";
    }

    H *= -1;
    fin.close();
    return H;
}

double getSize(string input)
{
    char ch;
    ifstream fin;
    fin.open(input, ios::binary);
    int byte = 0;
    while (fin.read(&ch, sizeof(char)))
    {
        byte++;
    }
    return byte / 1024.0;
}

int main()
{
    LZ77 lz5_4(5, 4);
    LZ77 lz10_8 (10, 8);
    LZ77 lz20_16(20, 16);
    ShannonFano sh;

    vector<string> files = {"../DATA/1.txt", "../DATA/2.docx", "../DATA/3.pptx", "../DATA/4.pdf",
                            "../DATA/5.exe", "../DATA/6.jpg", "../DATA/7.jpg",
                            "../DATA/8.bmp", "../DATA/9.bmp"};

    vector<string> names = {"../DATA/1", "../DATA/2", "../DATA/3", "../DATA/4",
                            "../DATA/5", "../DATA/6", "../DATA/7",
                            "../DATA/8", "../DATA/9"};

    ofstream fout, fout_freq;
    fout.open("result.txt");
    fout_freq.open("frequencies.txt");

    for (int i = 0; i < files[i].size(); ++i)
    {
        fout << files[i] << "\n";
        fout << "size: " << getSize(files[i]) << "\n";
        fout << "entropy: " << calc_entropy(fout_freq, files[i]) << "\n";

        auto encode_sh = time_measurement_encode_ShannonFano(sh, files[i], names[i] + ".shan");
        auto decode_sh = time_measurement_decode_ShannonFano(sh, names[i] + ".shan", names[i] + ".unshan");

        fout << "ShannonFano\nencode time: " << encode_sh.first << " coeff: "
                << encode_sh.second << "\n" << "decode time: " << decode_sh << "\n" <<
                "encode size: " << getSize(names[i] + ".shan");

        auto encode_lz77_5_4 = time_measurement_encode_LZ77(lz5_4, files[i], names[i] + ".lz775");
        auto decode_lz77_5_4 = time_measurement_decode_LZ77(lz5_4, names[i] + ".lz775", names[i] + ".unlz775");

        fout << "LZ775_4\nencode time: " << encode_lz77_5_4.first << " coeff: "
                << encode_lz77_5_4.second << "\n" << "decode time: " << decode_lz77_5_4 << "\n"
                << "encode size: " << getSize(names[i] + ".lz775");;

        auto encode_lz77_10_8 = time_measurement_encode_LZ77(lz10_8, files[i], names[i] + ".lz7710");
        auto decode_lz77_10_8 = time_measurement_decode_LZ77(lz10_8, names[i] + ".lz7710", names[i] + ".unlz7710");

        fout << "LZ7710_8\nencode time: " << encode_lz77_10_8.first << " coeff: "
             << encode_lz77_10_8.second << "\n" << "decode time: " << decode_lz77_10_8 << "\n"
             << "encode size: " << getSize(names[i] + "lz7710");

        auto encode_lz77_20_16 = time_measurement_encode_LZ77(lz20_16, files[i], names[i] + ".lz7720");
        auto decode_lz77_20_16 = time_measurement_decode_LZ77(lz20_16, names[i] + ".lz7720", names[i] + ".unlz7720");

        fout << "LZ7720_16\nencode time: " << encode_lz77_20_16.first << " coeff: "
             << encode_lz77_20_16.second << "\n" << "decode time: " << decode_lz77_20_16 << "\n"
             << "encode size: " << getSize(names[i] + "llz7720") << "\n\n";
    }

    return 0;
}