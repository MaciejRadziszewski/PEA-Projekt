#include <windows.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>
#include "ini.h"
using namespace std::chrono;
using namespace std;

int najkrotsza_sum(int** matrix, int num_wierzch)
{
    //Ustawienie wierzcholka startowego
    int s = 0;
    //Stworzenie wektora z wierzcholkami innymi niz startowy
    vector<int> wierzch;
    for(int i=0; i<num_wierzch; i++)
    {
        if(i != s)
        {
            wierzch.push_back(i);
        }
    }
    int n = wierzch.size();
    //Ustawienie wartosci najkrotszego cyklu na INT_MAX
    int najkrotsza = INT_MAX;
    //Petla wyszukujaca kazda kolejna permutacje leksykograficzna, az do konca permutacji
    do
    {
        int waga_cyklu = 0;

        int j = s;
        //Petla obliczająca wage cyklu aktualnej permutacji
        for (int i = 0; i < n; i++)
        {
            waga_cyklu += matrix[j][wierzch[i]];
            j = wierzch[i];
        }
        waga_cyklu += matrix[j][s];
        //Jezeli nowa waga jest mniejsza niz dotychczasowa najkrotsza, aktualna staje sie najkrotsza
        if(waga_cyklu < najkrotsza)
            najkrotsza = waga_cyklu;
    }
    while(next_permutation(wierzch.begin(),wierzch.end()));
    return najkrotsza;
}



int main()
{
    ofstream csvFile("wyniki.csv");
    mINI::INIFile file("plik.ini");
    mINI::INIStructure ini;
    file.read(ini);

    for(int iter = 3; iter < 8; iter++)
    {
        //Wejście danych
        int suma_cyklu;
        string name = to_string(iter);
        string fileName = ini[name]["fileName"];
        string solutions = ini[name]["solution"];
        int rozwiazanie = stoi(solutions);
        ifstream we;
        we.open(fileName);

        int num_wierzch;
        we >> num_wierzch;

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
        suma_cyklu = najkrotsza_sum(matrix, num_wierzch);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<nanoseconds>(stop - start);
        float time = (duration.count())/(1e+9);
        if(suma_cyklu == rozwiazanie)
        {
            //Wypisanie danych do pliku .csv
            cout << fileName << endl;
            cout << "Time [ms] : " << std::fixed << std::setprecision(5) << time*1000  <<"[ms]" << " Wynik: "<< suma_cyklu<< endl;
            csvFile << fileName << ";";
            csvFile << "Time [ms] = ;" << std::fixed << std::setprecision(5) << time*1000  << endl << endl;
        }
    }


    return 0;
}
