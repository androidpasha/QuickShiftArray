#include <Arduino.h>
#include "CyclicArray.h"

#define ARR_SIZE 9                        //Количество элементов массива. 
CyclicArray<int> arr{1,2,3,4,5,6,7,8,9};  // создаем массив интов размером ARR_SIZE на куче

template <typename T>                     //Функция выводит массив в Serial
void print(T &arr) {                      //передача по значению (вызываеться конструктор копии)
  for (auto it = std::begin(arr), end = std::end(arr); it != end; ++it)
    Serial.printf("%d ", *it);
  Serial.printf("\n");
}

void setup()
{
  Serial.begin(74880);

    auto it = std::begin(arr);
    std::advance(it, 4); // Переместить итератор на 4 позиции вправо
    Serial.printf("prev: %d advance: %d next: %d\n", *std::prev(it), *it, *std::next(it, 2)); // 4 5 7

    int distance = std::distance(std::begin(arr), std::end(arr));
    Serial.printf("distance: %d\n", distance); //distance: 9

    CyclicArray<int>::Iterator itB, itE; // или auto itB = std::begin(arr);
    {
        auto itB = std::begin(arr);
        auto itE = std::end(arr);
        Serial.printf("*itB=%d, *itE=%d\n", *itB, *itE); //*itB=1, *itE=9
        // Iterator	operator +
        Serial.printf("operator +\t");
        for (size_t i = 0; i < ARR_SIZE; i++)
            Serial.printf("%d ", *(itB + i));       // Iterator	operator +
        // Iterator	operator -
        Serial.printf("\noperator -\t");
        for (size_t i = 1; i <= ARR_SIZE; i++)
            Serial.printf("%d ", *(itE - i));       // Iterator	operator -
    }
    {
        arr >> 5;
        Serial.printf("\narr >> 5;");
        // Iterator	operator ++()
        Serial.printf("\noperator ++()\t"); 
        auto itB = std::begin(arr);
        auto itE = std::end(arr);
        for (size_t i = 0; i < ARR_SIZE; ++i) {
                Serial.printf("%d ", *(itB));
                ++itB;
        }    
        // Iterator	operator --()
        Serial.printf("\noperator --begin\t");
        itB = std::begin(arr);
        itE = std::end(arr);
        for (size_t i = 0; i < ARR_SIZE; ++i) { // Iterator	operator --()
            Serial.printf("%d ", *(itB));   // 9 8 7 6 5 4 3 2 1
            if (i!=ARR_SIZE-1)--itB;
        }
    }
    // Iterator	operator ==
    Serial.printf("\noperator \t");
    Serial.printf("==%d, !=%d\n", arr.begin() == arr.begin(), arr.begin() != arr.end());
    itB = arr.begin();
    itE = arr.end();
    do {
        ++itB;
        if (itB == itE)
            Serial.printf("\nbegin(%p) == end(%p)", (void*)itB, (void*)itE);         //begin(0x3fff2220) == end(0x3fff221c) соблюдаеться выход end за диапазон кругового массива arr[max + 1]
        if (itB != itE)
            Serial.printf("\n%dbegin(%p) != end(%p)", *itB, (void*)itB, (void*)itE); //9begin(0x3fff221c) != end(0x3fff221c) фактический конец и начало в круговом массиве совпадают. Но end должен быть на 1 больше
    } while (itB != itE);
}

void loop(){}