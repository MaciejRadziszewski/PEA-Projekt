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

struct Osobnik
{
    vector<int> sciezka;
    int wartosc;
};

struct sortowanie
{
    inline bool operator() (const Osobnik& osobnik1, const Osobnik& osobnik2)
    {
        return(osobnik1.wartosc < osobnik2.wartosc);
    }
};

int znajdzIndeks(Osobnik osobnik, int wartosc)
{
    for(int s = 0; s < osobnik.sciezka.size(); s++)
    {
        if(osobnik.sciezka[s] == wartosc)
        {
            return s;
        }
    }
    return -1;
}
void wypisz(vector <int> vec)
{
    for(int s = 0; s < vec.size(); s++)
    {
        cout << vec[s] << " ";
    }
    cout<<endl;
}

//Funkcja znajdujaca cykl hammiltona za pomoca algorytmu genetycznego
int Genetic (int n, int** matrix, int wielkoscPopulacji, double wspMutacji, int wynik)
{
    //Deklaracja zmiennych oraz wektorów koniecznych do poprawnego przebiegu programu
    srand(time(NULL));
    default_random_engine e;
    std::uniform_real_distribution<> dis(0, 1);
    int metodaSel = 0;
    int metodaKrzy = 1;
    int metodaMut = 1;
    int najkrotsza = INT_MAX;
    double blad = 400.0;
    double cel = 0;
    if(n < 400)
    {
        cel = 49.5;
    }
    else if(n >= 400)
    {
        cel = 149.5;
    }
    //Stworzenie startowej populacji
    int bezZmian = 0;
    vector<int> baza;
    for(int i = 0; i < n; i++)
    {
        baza.push_back(i);
    }
    vector<Osobnik> popPopulacja;
    for(int i = 0; i < wielkoscPopulacji; i++)
    {
        //Generacja nowych osobników w ilości określonej na początku programu
        Osobnik osobnik;
        osobnik.sciezka = baza;
        //Ustawienie permutacji wierzchołków na losową
        random_shuffle(osobnik.sciezka.begin(), osobnik.sciezka.end());
        osobnik.wartosc = 0;
        for (int k = 0; k < n - 1; k++)
        {
            osobnik.wartosc += matrix[osobnik.sciezka.at(k)][osobnik.sciezka.at(k + 1)];
        }
        osobnik.wartosc += matrix[osobnik.sciezka.at(n - 1)][osobnik.sciezka.at(0)];
        //Dodanie stworzonych osobników do wektora populacji
        popPopulacja.push_back(osobnik);

    }
    //Deklaracja zmiennych uzywanych do pomiaru czasu
    clock_t start, finish;
    start = clock();
    finish = clock();
    int pokolenie = 0;

    //Pętla generująca nowe generacje, do momentu upływu zadanego czasu
    while((double(finish - start)/CLOCKS_PER_SEC <= 600))
    {
        //Deklaracja wymaganych wektorów i zmiennych
        pokolenie++;
        vector<Osobnik> populacja = popPopulacja;
        vector<Osobnik> aktPopulacja;
        vector<int> matingPool;
        int turniejRozmiar = wielkoscPopulacji/10;
        //Jeśli metoda selekcji to 0, ma miejsce selekcja metodą matingPool, w przeciwnym wypadku następuje selekcja metodą turniejową
        if(metodaSel == 0)
        {

            sort(populacja.begin(), populacja.end(), sortowanie());
            int maks = populacja[wielkoscPopulacji-1].wartosc;
            for(int i = 0; i < wielkoscPopulacji; i++)
            {
                int przystosowanie = maks - populacja[i].wartosc;
                for(int j = 0; j < przystosowanie+1; j++)
                {
                    matingPool.push_back(i);
                }
            }
        }
        for(int i = 0; i < wielkoscPopulacji; i++)
        {

            Osobnik rodzic1;
            Osobnik rodzic2;
            //Znalezienie dwójki rodziców za pomocą metody matingPool
            if(metodaSel == 0)
            {
                rodzic1 = populacja[matingPool[rand() % matingPool.size() + 0]];
                rodzic2 = populacja[matingPool[rand() % matingPool.size() + 0]];
            }
            //Znalezienie dwójki rodziców za pomocą metody selekcji turniejowej
            if(metodaSel == 1)
            {
                rodzic1.wartosc = INT_MAX;
                rodzic2.wartosc = INT_MAX;
                Osobnik kandydat;
                for(int j = 0; j < turniejRozmiar; j++)
                {
                    kandydat = populacja[rand() % (populacja.size()) + 0];
                    if(kandydat.wartosc < rodzic1.wartosc)
                    {
                        rodzic1 = kandydat;
                    }
                }
                for(int j = 0; j < turniejRozmiar; j++)
                {
                    kandydat = populacja[rand() % (populacja.size()) + 0];
                    if(kandydat.wartosc < rodzic2.wartosc)
                    {
                        rodzic2 = kandydat;
                    }
                }
            }
            //Stworzenie osobnika dziecko
            Osobnik dziecko;

            vector<bool> odwiedzone;
            odwiedzone.resize(n);
            //Jeśli metoda krzyżowania to 0, ma miejsce krzyżowanie zmodyfikowanyą metodą OX, w przciwnym wypadku następuje
            //krzyżowanie PMX

            //Krzyżowanie dwójki rodziców zmodyfikowaną metodą OX
            if(metodaKrzy == 0)
            {
                for(int j = 0; j < n; j++)
                {
                    if(j < n/2)
                    {
                        dziecko.sciezka.push_back(rodzic1.sciezka[j]);
                        odwiedzone[rodzic1.sciezka[j]] = true;
                    }
                    if(j >= n/2 && !odwiedzone[rodzic2.sciezka[j]])
                    {
                        dziecko.sciezka.push_back(rodzic2.sciezka[j]);
                        odwiedzone[rodzic2.sciezka[j]] = true;
                    }
                }
                for(int j = 0; j < n; j++)
                {
                    if(!odwiedzone[j])
                    {
                        dziecko.sciezka.push_back(j);
                    }
                }
                dziecko.wartosc = 0;
                for (int k = 0; k < n - 1; k++)
                {
                    dziecko.wartosc += matrix[dziecko.sciezka[k]][dziecko.sciezka[k+1]];
                }
                dziecko.wartosc += matrix[dziecko.sciezka[n - 1]][dziecko.sciezka[0]];
                aktPopulacja.push_back(dziecko);
            }
            //Krzyżowanie dwójki rodziców metodą PMX
            if(metodaKrzy == 1)
            {
                Osobnik dziecko2;
                vector<bool> odwiedzone2;
                vector<int> mapping;
                vector<int> mapping2;
                odwiedzone2.resize(n);
                dziecko2.sciezka.resize(n);
                dziecko.sciezka.resize(n);
                for(int j = 0; j < n; j++)
                {
                    dziecko.sciezka[j] = -1;
                    dziecko2.sciezka[j] = -1;
                }
                int poczatek = rand() % (n-1) + 0;
                int koniec = rand() % (n-1) + 0;
                if(koniec < poczatek)
                    swap(poczatek, koniec);

                mapping.resize(n);
                mapping2.resize(n);

                for(int j = poczatek; j <= koniec; j++)
                {
                    dziecko.sciezka[j] = rodzic2.sciezka[j];
                    dziecko2.sciezka[j] = rodzic1.sciezka[j];
                    odwiedzone[dziecko.sciezka[j]] = true;
                    odwiedzone2[dziecko2.sciezka[j]] = true;
                    mapping[rodzic2.sciezka[j]] = rodzic1.sciezka[j];
                    mapping2[rodzic1.sciezka[j]] = rodzic2.sciezka[j];
                }

                for(int j = 0; j < n; j++)
                {
                    if(!odwiedzone[j] && !odwiedzone2[j])
                    {
                        int indeks1 = znajdzIndeks(rodzic1, j);
                        int indeks2 = znajdzIndeks(rodzic2, j);
                        dziecko.sciezka[indeks1] = j;
                        dziecko2.sciezka[indeks2] = j;
                        odwiedzone[j] = true;
                        odwiedzone2[j] = true;
                    }

                }

                int wartosc1 = 0;
                int wartosc2 = 0;
                for(int j = 0; j < poczatek; j++)
                {
                    if(rodzic1.sciezka[j] != dziecko.sciezka[j])
                    {
                        wartosc1 = rodzic1.sciezka[j];
                        while(odwiedzone[wartosc1])
                        {
                            wartosc1 = mapping[wartosc1];
                        }
                        dziecko.sciezka[j] = wartosc1;
                        odwiedzone[wartosc1] = true;
                    }
                    if(rodzic2.sciezka[j] != dziecko2.sciezka[j])
                    {
                        wartosc2 = rodzic2.sciezka[j];
                        while(odwiedzone2[wartosc2])
                        {
                            wartosc2 = mapping2[wartosc2];
                        }
                        dziecko2.sciezka[j] = wartosc2;
                        odwiedzone2[wartosc2] = true;
                    }
                }
                for(int j = koniec+1; j < n; j++)
                {
                    if(rodzic1.sciezka[j] != dziecko.sciezka[j])
                    {
                        wartosc1 = rodzic1.sciezka[j];
                        while(odwiedzone[wartosc1])
                        {
                            wartosc1 = mapping[wartosc1];
                        }
                        dziecko.sciezka[j] = wartosc1;
                        odwiedzone[wartosc1] = true;
                    }
                    if(rodzic2.sciezka[j] != dziecko2.sciezka[j])
                    {
                        wartosc2 = rodzic2.sciezka[j];
                        while(odwiedzone2[wartosc2])
                        {
                            wartosc2 = mapping2[wartosc2];
                        }
                        dziecko2.sciezka[j] = wartosc2;
                        odwiedzone2[wartosc2] = true;
                    }
                }

                dziecko.wartosc = 0;
                for (int k = 0; k < n - 1; k++)
                {
                    dziecko.wartosc += matrix[dziecko.sciezka.at(k)][dziecko.sciezka.at(k + 1)];
                }
                dziecko.wartosc += matrix[dziecko.sciezka.at(n - 1)][dziecko.sciezka.at(0)];

                dziecko2.wartosc = 0;
                for (int k = 0; k < n - 1; k++)
                {
                    dziecko2.wartosc += matrix[dziecko2.sciezka.at(k)][dziecko2.sciezka.at(k + 1)];
                }
                dziecko2.wartosc += matrix[dziecko2.sciezka.at(n - 1)][dziecko2.sciezka.at(0)];
                if(dziecko.wartosc > dziecko2.wartosc)
                {
                    aktPopulacja.push_back(dziecko2);
                }
                else
                {
                    aktPopulacja.push_back(dziecko);
                }
            }

        }

        //Pętla przechodząca po wszystkich osobnikach w populacji
        int losowa = 100;
        for(int j = 0; j < wielkoscPopulacji; j++)
        {
            //Jeśli metoda mutacji to 0, najstępuje mutacja osobnika metodą inversion
            losowa = dis(e);

            if(losowa < wspMutacji && metodaMut == 0)
            {
                int punkt1 = rand() % n + 0;
                int punkt2 = rand() % n + 0;
                if(punkt2 < punkt1)
                    swap(punkt1, punkt2);
                vector<int> losowanie;
                for(int i = punkt1; i <= punkt2; i++)
                {
                    losowanie.push_back(aktPopulacja[j].sciezka[i]);
                }
                int licznik = 0;
                random_shuffle(losowanie.begin(), losowanie.end());

                for(int i = punkt1; i <= punkt2; i++)
                {
                    aktPopulacja[j].sciezka[i] = losowanie[licznik];
                    licznik ++;
                }
                aktPopulacja[j].wartosc = 0;
                for (int k = 0; k < n - 1; k++)
                {
                    aktPopulacja[j].wartosc += matrix[aktPopulacja[j].sciezka.at(k)][aktPopulacja[j].sciezka.at(k + 1)];
                }
                aktPopulacja[j].wartosc += matrix[aktPopulacja[j].sciezka.at(n - 1)][aktPopulacja[j].sciezka.at(0)];
            }
            //Jeśli metoda mutacji to 1, najstępuje mutacja osobnika metodą transposition
            if(losowa < wspMutacji && metodaMut == 1)
            {
                int punkt1 = rand() % (n-1) + 0;
                int punkt2 = rand() % (n-1) + 0;
                swap(aktPopulacja[j].sciezka[punkt1],aktPopulacja[j].sciezka[punkt2]);
                aktPopulacja[j].wartosc = 0;
                for (int k = 0; k < n - 1; k++)
                {
                    aktPopulacja[j].wartosc += matrix[aktPopulacja[j].sciezka.at(k)][aktPopulacja[j].sciezka.at(k + 1)];
                }
                aktPopulacja[j].wartosc += matrix[aktPopulacja[j].sciezka.at(n - 1)][aktPopulacja[j].sciezka.at(0)];
            }
        }
        //Sortowanie nowej populacji
        sort(aktPopulacja.begin(), aktPopulacja.end(), sortowanie());
        //sort(populacja.begin(), populacja.end(), sortowanie());


        if(aktPopulacja[0].wartosc == najkrotsza)
        {
            bezZmian++;
            najkrotsza = aktPopulacja[0].wartosc;
        }
        else if (aktPopulacja[0].wartosc < najkrotsza)
        {
            bezZmian = 0;
            najkrotsza = aktPopulacja[0].wartosc;
        }
        /*
        int pom = wielkoscPopulacji/15;
        for(int i = 0; i < pom; i++)
        {
            aktPopulacja.push_back(populacja[i]);
        }
        wielkoscPopulacji = wielkoscPopulacji + wielkoscPopulacji/10;
        */
        popPopulacja = aktPopulacja;
        finish = clock();
        blad = (((double)najkrotsza - (double)wynik)/(double)wynik)*100;
    }

    return najkrotsza;

}

int main()
{
    ofstream csvFile("wyniki.csv");
    mINI::INIFile file("plik.ini");
    mINI::INIStructure ini;
    file.read(ini);

    for(int iter = 1; iter < 16; iter++)
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
        int wielkosPop;
        if(num_wierzch < 70)
        {
            wielkosPop = num_wierzch*3;
        }
        else if(num_wierzch <= 170)
        {
            wielkosPop = num_wierzch/1.5;
        }
        else if(num_wierzch < 300)
        {
            wielkosPop = num_wierzch/4;
        }
        else
        {
            wielkosPop = num_wierzch/7;
        }
        //Pomiar czasu
        auto start = high_resolution_clock::now();
        //Algorytm znajdowania najkrotszego cyklu

        suma_cyklu = Genetic(num_wierzch, matrix, wielkosPop, 0.15, rozwiazanie);
        auto stop = high_resolution_clock::now();
        auto duration = duration_cast<nanoseconds>(stop - start);
        float time = (duration.count())/(1e+9);
        double blad = (((double)suma_cyklu - (double)rozwiazanie)/(double)rozwiazanie)*100;
        //Wypisanie danych do pliku .csv
        cout << fileName << endl;
        cout << "Time [ms] : " << std::fixed << std::setprecision(3) << time*1000  <<"[ms]" <<"Wynik: " << suma_cyklu << " Rozwiazanie: " << rozwiazanie << " Blad: "<< blad <<"%"<< endl;
        csvFile << fileName << ";";
        csvFile << "Time [ms] = ;" << std::fixed << std::setprecision(3) << time*1000  << ";"<< blad << endl;

    }


    return 0;
}
