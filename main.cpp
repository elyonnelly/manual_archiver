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
#include "ShannonFano.h"
#include "LZ77.h"

using namespace std;

int main()
{
    /*chrono::time_point<chrono::system_clock> start, end;
    start = chrono::system_clock::now();
    end = chrono::system_clock::now();
    int elapsed_seconds = chrono::duration_cast<chrono::nanoseconds>
            (end-start).count();
    cout << elapsed_seconds << "mc \n";*/

    auto * shf = new ShannonFano();
    shf->encode("input.txt", "output.txt");
    cout << "\n";
    shf->decode("output.txt", "decode.txt");

    return 0;
}