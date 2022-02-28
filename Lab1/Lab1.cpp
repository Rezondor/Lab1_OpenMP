#include <iostream>
#include <stdio.h>
#include <string>
#include <random>
#include <ctime>
#include <omp.h>
#include <vector>

using namespace std;

const int SIZE[] = { 10000, 50000, 100000 };
const int THREADCOUNT[] = { 2, 5,10 };
const int RANDOM = 100000;
const int ArrayCount = 3;
static int cn = 0; 
static int numb = 0;

void BubbleSort(int data[], int lenD);
void SelectionSort(int data[], int lenD);
void InsertionSort(int data[], int lenD);
typedef void (*MySort) (int[], int);
const MySort TypeSort[] = { BubbleSort,SelectionSort,InsertionSort };

void BubbleSort(int data[], int lenD)
{
    int tmp = 0;
    for (int i = 0;i < lenD;i++) {
        for (int j = (lenD - 1);j >= (i + 1);j--) {
            if (data[j] < data[j - 1]) {
                tmp = data[j];
                data[j] = data[j - 1];
                data[j - 1] = tmp;
            }
        }
    }
}
void SelectionSort(int data[], int lenD)
{
    int j = 0;
    int tmp = 0;
    for (int i = 0; i < lenD; i++) {
        j = i;
        for (int k = i; k < lenD; k++) {
            if (data[j] > data[k]) {
                j = k;
            }
        }
        tmp = data[i];
        data[i] = data[j];
        data[j] = tmp;
    }
}
void InsertionSort(int data[], int lenD)
{
    int key = 0;
    int i = 0;
    for (int j = 1;j < lenD;j++) {
        key = data[j];
        i = j - 1;
        while (i >= 0 && data[i] > key) {
            data[i + 1] = data[i];
            i = i - 1;
            data[i + 1] = key;
        }
    }
}
void SortParallel(int data[], int lenD)
{
    if (THREADCOUNT[numb] >= lenD || THREADCOUNT[numb]==1|| THREADCOUNT[numb]<=0) {
        TypeSort[cn % 3](data, lenD);
        return;
    }
    vector<int> *thread = new vector<int>[THREADCOUNT[numb]];
    int rnd = RANDOM / THREADCOUNT[numb];
    for (size_t i = 0; i < THREADCOUNT[numb]; i++)
    {
        int start = i * rnd;
        int end = (i + 1) * rnd;
        for (size_t j = 0; j < lenD; j++)
        {
            if (data[j] < end && data[j] >= start)
            {
                thread[i].push_back(data[j]);
            }
        }
    }// Разбиение на подмассивы

#pragma omp parallel for
    for (int i = 0; i < THREADCOUNT[numb]; i++)
    {
        int sz = thread[i].size();
        int* arr = new int[sz];
        int count = 0;
        int start = 0;

        for (auto a : thread[i])
        {
            arr[count] = a;
            count += 1;
        }

        TypeSort[cn % 3](arr, sz);
        

        for (size_t j = 0; j < i; j++)
        {
            start += thread[j].size();
        }

        for (size_t j = start, k = 0; j < sz + start; j++, k++)
        {
            data[j] = arr[k];

        }
        delete[] arr;
    }

    delete[] thread;
}
void Print(int a[],int size) 
{
    cout << "Массив:"<<endl;
    for (int i =0; i<size; i++)
    {
        cout << a[i] << " ";
    }
    cout << endl;
}
void Completion(int first[], int second[], int size)
{
    for (size_t i = 0; i < size; i++)
    {
        int a = first[i];
        second[i] = a;
    }
}



int main()
{
    setlocale(LC_ALL, "ru");
    srand(1);
    float TimeAllPos[9];
    float TimeAllPar[27];
    int TimeAllPosNow = 0;
    int TimeAllParNow = 0;
    for (int g = 0; g < 3; g++)
    {
        numb = g;
        for (int f = 0; f < 3;f++) {
            
            int* StaticArray = new int[SIZE[f]];
            int* StaticArraySort = new int[SIZE[f]];
            int** array = new int* [ArrayCount];
            for (int i = 0; i < ArrayCount;i++)
                array[i] = new int[SIZE[f]];

            int start_time;
            int end_time;
            int search_time;
            float tm = 0.0;
            omp_set_num_threads(THREADCOUNT[g]);
            printf("\nКол-во потоков - %d Размер масива - %d\n\n", THREADCOUNT[g], SIZE[f]);
            for (auto i = 0; i < SIZE[f]; i++)
            {
                int rn = rand() % 100;
                StaticArray[i] = (rand() * rn) % RANDOM;
            }

            Completion(StaticArray, StaticArraySort, SIZE[f]);
            InsertionSort(StaticArraySort, SIZE[f]);
            if(g==0)
            for (int i = 0; i < ArrayCount; i++)
            {
                cn = i;
                start_time = 0;
                tm = 0;
                for (int j = 0; j < 5; j++)
                {
                    Completion(StaticArray, array[i], SIZE[f]);

                    int st_time = clock();
                    TypeSort[i % 3](array[i], SIZE[f]);
                    int ed_time = clock();
                    int sch_time = ed_time - st_time;
                    float aa = sch_time / 1000.0;
                    switch (i % 3)
                    {
                    case 0:

                        printf("Сортировка пузырьком - %.3f", aa);
                        break;
                    case 1:

                        printf("Сортировка выбором   - %.3f", aa);
                        break;
                    case 2:

                        printf("Сортировка вставками  - %.3f", aa);
                        break;
                    default:
                        break;
                    }
                    tm += aa;
                    if (equal(array[i], array[i] + SIZE[f], StaticArraySort)) cout << " Сортировка выполнена верно!\n";
                    else cout << " Сортировка выполнена не верно!\n";

                }

                cout << "Среднее время последовательной сортировки - " << tm / 5.0 << endl << endl;
                TimeAllPos[TimeAllPosNow] = tm / 5.0;
                TimeAllPosNow += 1;
            }

            for (int i = 0; i < ArrayCount; i++)
            {
                cn = i;
                start_time = 0;
                tm = 0;
                for (int j = 0; j < 5; j++)
                {
                    Completion(StaticArray, array[i], SIZE[f]);

                    int st_time = clock();
                    SortParallel(array[i], SIZE[f]);
                    int ed_time = clock();
                    int sch_time = ed_time - st_time;
                    float aa = sch_time / 1000.0;
                    switch (i % 3)
                    {
                    case 0:

                        printf("Сортировка пузырьком - %.3f", aa);
                        break;
                    case 1:

                        printf("Сортировка выбором   - %.3f", aa);
                        break;
                    case 2:

                        printf("Сортировка вставками  - %.3f", aa);
                        break;
                    default:
                        break;
                    }   
                    tm += aa;
                    if (equal(array[i], array[i] + SIZE[f], StaticArraySort)) cout << " Сортировка выполнена верно!\n";
                    else cout << " Сортировка выполнена не верно!\n";

                }
                cout << "Среднее время параллельной   сортировки - " << tm / 5.0 << endl << endl;
                TimeAllPar[TimeAllParNow] = tm / 5.0;
                TimeAllParNow += 1;
            }


            delete[] StaticArray;
            delete[] StaticArraySort;
            for (size_t i = 0; i < ArrayCount; i++)
            {
                delete[] array[i];
            }

        }
    }
    printf("\n|%10s|%10s|%10s|%10s|%10s|%10s|%10s|%10s|%10s|%10s|%10s|%10s|%10s|\n",
        " ", "10К-1", "50К-1", "100К-1", "10К-2", "50К-2", "100К-2", "10К-5", "50К-5", "100К-5", "10К-10", "50К-10", "100К-10");
    for(int i = 0;i<3;i++)
    {
        switch (i)
        {
            case 0:
                printf("|%10s|", "Пузырёк");
                break;
            case 1:
                printf("|%10s|", "Выбором");
                break;
            case 2:
                printf("|%10s|", "Вставками");
                break;
            default:
                break;
        }
        for (int j = i;j < 9;j += 3)
            printf("%10.3f|", TimeAllPos[j]);
        for (int j = i;j < 27;j += 3)
            printf("%10.3f|", TimeAllPar[j]);
        printf("\n");
    }
   
}

