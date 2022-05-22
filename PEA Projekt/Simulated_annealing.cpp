#include <windows.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include <tgmath.h>
#include <random>
#include "ini.h"
using namespace std::chrono;
using namespace std;

//Funkcja znajdujaca cykl hammiltona za pomoca algorytmu symulowanego wyżarzania
int Sim_ann (int n, int** matrix, float temperatura, float chlodzenie)
{
    //Deklaracja wektorów przechowywujących optymalne ciągi dla konkretnych momentów algorytmu
    vector <int> wierzch(n);
    vector <int> tym_wierzch(n);
    vector <int> epoka_min(n);
    //Deklaracja wartości służących do generowania losowych wartości
    srand(time(NULL));
    default_random_engine e;
    std::uniform_real_distribution<> dis(0, 1);

    //Stworzenie pierwszej permutacji w wektorze wierzch
    for (int i = 0; i < n; i++)
        wierzch[i] = i;

    random_shuffle(wierzch.begin(), wierzch.end());

    //Deklaracja zmiennej przechowywującej najkrótszą drogę, oraz przypisanie jej początkowej wartości
    int najlepsza = 0;
    for (int i = 0; i < n; i++)
    {
        if( i + 1 != n)
            najlepsza += matrix[wierzch[i]][wierzch[i + 1]];
        else
            najlepsza += matrix[wierzch[i]][wierzch[0]];

    }

    //Deklaracja zmiennych używanych przy liczeniu minimum w epoce
    double tym_min = 0;
    double temperatura_najmniejsza = 0.01;
    int dlg_epoki = n;
    float P = 0;

    int epoka_sum_min = INT_MAX;
    //Dopóki temperatura nie osiągnie przewidywanej wartości najmniejszej, algorytm będzie się wykonywać
    while(temperatura > temperatura_najmniejsza)
    {
        //Przypisanie wektora wierzch do wektora epoka_min
        int epoka = 0;
        epoka_min = wierzch;
        //Pętla wykonywująca się na przestrzeni jednej epoki
        for(int k = 0; k < dlg_epoki; k++)
        {
            //Przypisanie wektora epoka_min do tym_wierzch
            tym_wierzch = epoka_min;
            //Generowanie dwóch losowych wierchołków, które zostaną zamienione w wektorze epoka_min
            int losowyWierzch1 = rand() % (n-1) + 0;
            int losowyWierzch2 = rand() % (n-1) + 0;
            while (losowyWierzch1 == losowyWierzch2)
                losowyWierzch2 = rand() % (n-1) + 0;
            //Zamiana wierzchołków w wektorze
            swap(tym_wierzch[losowyWierzch1], tym_wierzch[losowyWierzch2]);
            int tym_min = 0;
            //Pętla licząca sumę wag cyklu dla tymczasowej permutacji wierzchołków
            for (int i = 0; i < n; i++)
            {
                if( i + 1 != n)
                    tym_min += matrix[tym_wierzch[i]][tym_wierzch[i+1]];
                else
                    tym_min += matrix[tym_wierzch[i]][tym_wierzch[0]];

            }
            //Warunek sprawdzający, czy suma wag w cyklu dla tymczasowej permutacji jest mniejsza niż suma wag w cyklu dla
            //najlepszej permutacji w epoce
            if (tym_min < epoka_sum_min)
                P = 1;
            //W przeciwnym wypadku sprawdzany jest warunek przyjęcia gorszej wartości zależny od temperatury i różnicy wyników
            else
                P = exp(-(tym_min - epoka_sum_min)/temperatura);
            //Jeżeli jeden z powyższych warunków jest poprawny, to tymczasowe minimum i permutacja staje się minimum i najlepszą
            //permutacją w epoce
            if (P > dis(e))
            {
                epoka_sum_min = tym_min;
                swap(epoka_min, tym_wierzch);
            }

        }
        //Przejście do kolejnej epoki
        epoka ++;
        //Warunek sprawdzający, czy suma wag w cyklu dla najlepszej permutacji w epoce jest mniejsza niż najlepsza suma wag
        //obliczona do tego momentu w algorytmie
        if (epoka_sum_min < najlepsza)
            P = 1;
        //W przeciwnym wypadku sprawdzany jest warunek przyjęcia gorszej wartości zależny od temperatury i różnicy wyników
        else
            P = exp(-(epoka_sum_min - najlepsza)/temperatura);
        //Jeżeli jeden z powyższych warunków jest poprawny, to minimum epoki i jej najlepsza permutacja stają się
        //minimum i najlepszą permutacją dotychczas
        if (P > dis(e))
        {
            najlepsza = epoka_sum_min;
            swap(epoka_min, wierzch);
        }
        //Chłodzenie geometryczne, którego prędkość zależy od zmiennej chlodzenie
        temperatura *= chlodzenie;
    }
    //Zwrócenie najlepszej znalezionej sumy w cyklu
    return najlepsza;
}

int main()
{
    ofstream csvFile("wyniki.csv");
    mINI::INIFile file("plik.ini");
    mINI::INIStructure ini;
    file.read(ini);

    for(int iter = 1; iter < 7; iter++)
    {
        size_t size;
        //Wejœcie danych
        int suma_cyklu;
        string name = to_string(iter);
        string fileName = ini[name]["fileName"];
        string solutions = ini[name]["solution"];
        int rozwiazanie = stoi(solutions);
        ifstream we;
        we.open(fileName);


        int num_wierzch;
        we >> num_wierzch;

        float tempratura = 100 * num_wierzch;
        float chlodzenie = 0.9999;

        // Deklaracja wskaznika na macierz sasiedztwa
        int** matrix = new int*[num_wierzch];
        // Wypelnienie macierzy sasiedztwa
        for(int i=0; i<num_wierzch; i++)
        {
            matrix[i] = new int[num_wierzch];
            for(int j=0; j<num_wierzch; j++)
            {
                we >> matrix[i][j];
            }
        }

        //Pomiar czasu
        auto start = high_resolution_clock::now();
        //Algorytm znajdowania najkrotszego cyklu
        suma_cyklu = Sim_ann(num_wierzch, matrix, tempratura, chlodzenie);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<nanoseconds>(stop - start);
        float time = (duration.count())/(1e+9);
        double blad = ((double)suma_cyklu - (double)rozwiazanie)/((double)rozwiazanie/100);
        //Wypisanie danych do pliku .csv
        cout << fileName << endl;
        cout << "Time [ms] : " << std::fixed << std::setprecision(3) << time*1000  <<"[ms]" <<"Wynik: " << suma_cyklu << " Blad: "<< blad <<"%"<< endl;
        csvFile << fileName << ";";
        csvFile << "Time [ms] = ;" << std::fixed << std::setprecision(3) << time*1000  << ";"<< blad << endl << endl;

    }


    return 0;
}
