#include <Arduino.h>
#include "CyclicArray.h"
#include <array>

#define ARR_SIZE 9                     //Количество элементов массива. 
CyclicArray<int> arr(ARR_SIZE);         // создаем массив интов размером ARR_SIZE на куче

template <typename T>                   //Функция выводит массив в Serial
void print(T arr){                      //передача по значению (вызываеться конструктор копии)
  for (auto &e : arr)
    Serial.printf("%d ", e);
  Serial.println();
}

void setup()
{
  Serial.begin(74880);
              Serial.println();

  for (size_t i = 0; i < ARR_SIZE; i++)
      arr[i]=i+1;

  for(auto it=arr.begin(), end = arr.end(); it!=end; ++it)
      Serial.printf("%d ", *it); // 1 2 3 4 5 6 7 8 9
              Serial.println();

  for(auto it=std::begin(arr), end = std::end(arr); it!=end; ++it)
      Serial.printf("%d ", *it); // 1 2 3 4 5 6 7 8 9
              Serial.println(); 
  auto it=std::begin(arr);
  std::advance(it, 4); // Переместить итератор на 4 позиции вправо
  Serial.printf("prev: %d advance: %d next: %d\n",*std::prev(it), *it, *std::next(it, 2)); // 4 5 7

  int distance = std::distance(std::begin(arr), std::end(arr));
  Serial.printf("distance: %d\n", distance);

              
  arr << 5;
  print(arr);//6 7 8 9 =1=-1 1 2 3 4 

  CyclicArray<int>::Iterator itB, itE;

  arr >> 5;
  itB = arr.begin();
  itE = arr.end();
  Serial.printf("*itB=%d, *itE=%d\n", *itB, *itE);
                                          Serial.print("operator +\t");
  for (size_t i = 0; i < ARR_SIZE; i++)
    Serial.printf("%d ",*(itB+i));       // Iterator	operator +
                                          Serial.print("\noperator -\t");
  for (size_t i = 1; i <= ARR_SIZE; i++)
    Serial.printf("%d ",*(itE-i));       // Iterator	operator -
  
                                        Serial.print("\noperator ++()\t");
   itB = arr.begin();
  itE =  arr.end();
  for (; itB!=itE; ++itB)
    Serial.printf("%d ",*(itB));       // Iterator	operator ++()
                                          Serial.print("\noperator --()\t");
  itB = arr.begin() + ARR_SIZE - 1;
  itE =  arr.end();
  for (; itB!=itE; --itB){
    Serial.printf("%d ",*(itB));       // Iterator	operator --()
  --itE ;                              
  }                               Serial.print("\noperator !=\t");
  //arr << 5;
  itB = arr.begin();
  itE =  arr.end(); 
  for (size_t i = 0; i <= ARR_SIZE+1; i++)
    Serial.printf("%d=(%d!=%d); ", (++itB != itE), *itB, *itE);       // Iterator	operator !=
  

                                            Serial.print("\noperator ==\t");
  itB = arr.begin();
  itE =  arr.end(); 
             
   for (size_t i = 0; i <= ARR_SIZE; i++)
     Serial.printf("%d=(%d==%d); ", (++itB == itE), *itB, *itE);       // Iterator	operator ==
  

}

void loop(){ yield(); }