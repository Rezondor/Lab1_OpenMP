#include <iostream>
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
void QuickSort(int data[], int len);
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
    if (THREADCOUNT[numb] >= lenD) {
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
    float TimeAll[18];
    int TimeAllNow = 0;

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
            if(f==0)
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
                TimeAll[TimeAllNow] = tm / 5.0;
                TimeAllNow += 1;
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

                        printf("Сортировка пузырьком  - %.3f", aa);
                        break;
                    case 1:

                        printf("Сортировка выбором    - %.3f", aa);
                        break;
                    case 2:

                        printf("Быстрая сортировка    - %.3f", aa);
                        break;
                    default:
                        break;
                    }
                    tm += aa;
                    if (equal(array[i], array[i] + SIZE[f], StaticArraySort)) cout << " Сортировка выполнена верно!\n";
                    else cout << " Сортировка выполнена не верно!\n";

                }
                cout << "Среднее время параллельной   сортировки - " << tm / 5.0 << endl << endl;
                TimeAll[TimeAllNow] = tm / 5.0;
                TimeAllNow += 1;
            }


            delete[] StaticArray;
            delete[] StaticArraySort;
            for (size_t i = 0; i < ArrayCount; i++)
            {
                delete[] array[i];
            }

        }
    }
  
    for (int i = 0; i < 18; i++)
    {
        printf("Время - %.3f", TimeAll[i]);
        cout << endl;
    }
}

