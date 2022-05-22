#include <windows.h>
#include <chrono>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <vector>
#include "ini.h"
using namespace std::chrono;
using namespace std;

//Funkcja znajdujaca cykl hammiltona za pomoca algorytmu helda-karpa
int heldkarp (int N, int** matrix)
{
    //Deklaracja wektora przechowywyujacego podzbior S w postaci int, oraz wartość najbardziej optymalnej trasy dla poszczegolnego ostatniego wierzchołka
    vector<vector<int>> best (1<<(N-1),vector<int>(N,INT_MAX));
    //Petla przechodzaca po wszystkich mozliwych liczbach od 1 do 2^(n-1), które reprezentuja poszczegolne podzbiory
    for (int odwiedzone = 1; odwiedzone < (1<<(N-1)); ++odwiedzone)
    {
        //Petla przechodzaca po wszystkich mozliwych ostatnich wierzcholkach w podzbiorze odwiedzone
        for (int ostatni = 0; ostatni < (N-1); ++ostatni)
        {

            //Warunek, ktory zapewnia, ze ostatni odwiedzany wierzcholek jest w podzbiorze odwiedzone
            if (!(odwiedzone & 1<<ostatni)) continue;

            //Warunek, ktory sprawdza czy odwiedzany wierzcholek jest jedynym w podzbiorze odwiedzone
            if (odwiedzone == 1 << ostatni)
            {
                //Jesli odwiedzany wierzcholek jest jedynym w podzbiorze odwiedzone to wartosc jego najbardziej optymalnej trasy ustawiana jest na odledleglosc jego od wierzcholka poczatkowego n-1
                best[odwiedzone][ostatni] = min(matrix[N-1][ostatni],
                                                matrix[ostatni][N-1]);
            }
            else
            {
                //Jesli wierzcholek znajduje sie w podzbiorze S i nie jest jedynym wierzcholkiem w tym podzbiorze,
                //to znajdowany jest podzbior prev_odwiedzone,czyli odwiedzone ale bez ostatnio odwiedzonego wierzcholka
                int prev_odwiedzone = odwiedzone ^ 1<<ostatni;
                //Petla przechodzaca po wszystkich mozliwych ostatnich wierzcholkach w podzbiorze prev_odwiedzone
                for (int prev = 0; prev < N-1; ++prev)
                {
                    //Warunek, ktory zapewnia, ze odwiedzany wierzcholek jest w podzbiorze prev_odwiedzone
                    if (!(prev_odwiedzone & 1<<prev)) continue;
                    //Jesli wierzcholek znajduje sie zajdowana jest nowa wartosc optymalnej trasy dla podzbioru odwiedzone, gdzie ostatnim wierzcholkiem jest ostatni
                    best[odwiedzone][ostatni] = min(
                                              //Nowa wartosc optymalnej trasy znajdowana jest przez porownanie aktualnej optymalnej trasy do
                                              //sumy wartości optymalnej trasy w zbiorze prev_odwiedzone oraz drogi od ostatniego wierzcholka prev_odwiedzone czyli prev do wierzcholka ostatni
                                              best[odwiedzone][ostatni],
                                              min(matrix[ostatni][prev],matrix[prev][ostatni])  + best[prev_odwiedzone][prev]
                                          );
                }
            }
        }
    }

    //deklaracja zmiennej przechowywujacej wartosc najmniejszego cykl hammiltona w grafie
    int odpowiedz = INT_MAX;
    //Petla przechodzaca po wszystkich ostatnich wierzcholkach w przedostatnim podzbiorze
    for (int ostatni=0; ostatni<N-1; ++ostatni)
    {
        //Nowa wartosc najmniejszego cyklu hammiltona znajdowana jest przez porownywanie aktualnej odpowiedzi do
        //sumy wartosci optymalej trasy w przedostatnim zbiorze oraz drogi od ostatniego wierzcholka jakim jest ostatni w przedostatnim zbiorze do wierzcholka n-1
        odpowiedz = min(
                     odpowiedz,
                     matrix[ostatni][N-1] + best[ (1<<(N-1))-1 ][ostatni]
                 );
    }
    //Funkcja zwraca wartosc najmniejszego cyklu hammiltona
    return odpowiedz;
}
int main()
{
    ofstream csvFile("wyniki.csv");
    mINI::INIFile file("plik.ini");
    mINI::INIStructure ini;
    file.read(ini);

    for(int iter = 1; iter < 8; iter++)
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
        suma_cyklu = heldkarp(num_wierzch, matrix);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<nanoseconds>(stop - start);
        float time = (duration.count())/(1e+9);
        //if(suma_cyklu == rozwiazanie)
        //{
            //Wypisanie danych do pliku .csv
            cout << fileName << endl;
            cout << "Time [ms] : " << std::fixed << std::setprecision(3) << time*1000  <<"[ms]" <<"Wynik: " << suma_cyklu << endl;
            csvFile << fileName << ";";
            csvFile << "Time [ms] = ;" << std::fixed << std::setprecision(3) << time*1000  << endl << endl;
        //}
    }


    return 0;
}
