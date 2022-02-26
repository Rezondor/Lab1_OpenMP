#include <iostream>
#include <string>
#include <random>
#include <ctime>
#include <omp.h>
#include <vector>

using namespace std;
const int SIZE = 50000;
const int RANDOM = 10000;
const int ArrayCount = 3;
const int THREADCOUNT = 10;
static int cn = 0; 

void BubbleSort(int data[], int lenD);
void SelectionSort(int data[], int lenD);
void QuickSort(int data[], int len);
typedef void (*MySort) (int[], int);
const MySort TypeSort[] = { BubbleSort,SelectionSort,QuickSort };

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
}//Вставки
void QuickSort(int data[], int len)
{
    int const lenD = len;
    int pivot = 0;
    int ind = lenD / 2;
    int i, j = 0, k = 0;
    if (lenD > 1) {
        int* L = new int[lenD];
        int* R = new int[lenD];
        pivot = data[ind];
        for (i = 0;i < lenD;i++) {
            if (i != ind) {
                if (data[i] < pivot) {
                    L[j] = data[i];
                    j++;
                }
                else {
                    R[k] = data[i];
                    k++;
                }
            }
        }
        QuickSort(L, j);
        QuickSort(R, k);
        for (int cnt = 0;cnt < lenD;cnt++) {
            if (cnt < j) {
                data[cnt] = L[cnt];;
            }
            else if (cnt == j) {
                data[cnt] = pivot;
            }
            else {
                data[cnt] = R[cnt - (j + 1)];
            }
        }
    }
}
void SortParallel(int data[], int lenD)
{
    vector<int> thread[THREADCOUNT];
    int rnd = RANDOM / THREADCOUNT;
    for (size_t i = 0; i < THREADCOUNT; i++)
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
    for (int i = 0; i < THREADCOUNT; i++)
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
        cn += 1;

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

    int StaticArray[SIZE];
    int StaticArraySort[SIZE];
    int array[ArrayCount][SIZE];

    int start_time;
    int end_time;
    int search_time;
    float tm = 0.0;
    omp_set_num_threads(THREADCOUNT);

    for (auto i = 0; i < SIZE; i++)
    {
        StaticArray[i] = rand() % RANDOM;
    }

    Completion(StaticArray, StaticArraySort, SIZE);
    QuickSort(StaticArraySort, SIZE);


    for (auto j = 0; j < 5; j++)
    {
        for (auto i = 0; i < ArrayCount; i++)
        {
            Completion(StaticArray, array[i], SIZE);
        }
        cout << "Сортировка - " << j + 1 << endl;
        start_time = clock();
        for (auto i = 0; i < ArrayCount; i++)
        {
            int st_time = clock();
            TypeSort[i%3](array[i], SIZE);
            int ed_time = clock();
            int sch_time = ed_time - st_time;
            float aa = sch_time / 1000.0;
            switch (i%3)
            {
            case 0:

                printf("Сортировка выбором   - %.3f", aa);
                break;
            case 1:

                printf("Сортировка вставками - %.3f", aa);
                break;
            case 2:

                printf("Быстрая сортировка   - %.3f", aa);
                break;
            default:
                break;
            }
            if (equal(array[i],array[i]+SIZE,StaticArraySort)) cout << " Сортировка выполнена верно!\n";
            else cout << " Сортировка выполнена не верно!\n";
        }
        end_time = clock();

        search_time = end_time - start_time;
        tm += search_time / 1000.0;
        cout << "Время выполнения     - " << search_time / 1000.0 << endl << endl;
    }
    cout << "Среднее время последовательных вычислений - " << tm / 5.0 << endl<<endl;


    tm = 0.0;
    for (auto j = 0; j < 5; j++)
    {
        for (auto i = 0; i < ArrayCount; i++)
        {
            Completion(StaticArray, array[i], SIZE);
        }
        cout << "Сортировка - " << j + 1 << endl;
        start_time = clock();
         
        for (auto i = 0; i < ArrayCount; i++)
        {
            int st_time = clock();
            SortParallel(array[i], SIZE);
            int ed_time = clock();
            int sch_time = ed_time - st_time;
            float aa = sch_time / 1000.0;
             switch (i%3)
             {
             case 0:

                 printf("Сортировка выбором   - %.3f ", aa);
                 break;
             case 1:

                 printf("Сортировка вставками - %.3f ", aa);
                 break;
             case 2:

                 printf("Быстрая сортировка   - %.3f ", aa);
                 break;
             default:
                 break;
             }
             if (equal(array[i],array[i]+SIZE,StaticArraySort)) cout << " Сортировка выполнена верно!\n";
             else cout << " Сортировка выполнена не верно!\n";
        }
        end_time = clock();

        search_time = end_time - start_time;
        tm += search_time / 1000.0;
        cout << "Время выполнения     - " << search_time / 1000.0 << endl << endl;
    }
    cout << "Среднее время паралельных вычислений - " << tm / 5.0 << endl;
    cout << "Время выполнения программы - " << clock()/1000.0;
}

