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

 //std::array<int, 10>::iterator it; 

  CyclicArray<int>sortArr{-1,1,-3,3,-2,2,-5,5,-4,4};
  print(sortArr);
  sortArr << 5;
  print(sortArr);
  std::sort(sortArr.begin(), sortArr.end(), std::greater<int>());  
  print(sortArr);     // 
  sortArr.setOffset(0);
  print(sortArr);
              Serial.println();





  arr.setOffset(0);
  for (size_t i = 0; i < ARR_SIZE; i++)
      arr[i]=i+1;

  print(arr);
  arr << 5;
  print(arr);//6 7 8 9 =1=-1 1 2 3 4 //this=6 other = 5.
  //this:1!=other:-1: 1 this:2!=other:-1: 2 this:3!=other:-1: 3 this:4!=other:-1: 4 this:5!=other:-1: 5 this:6!=other:-1: 6 this:7!=other:-1: 7 this:8!=other:-1: 8 this:9!=other:-1: 9 =1=this:-1!=other:-1:
//this:6!=other:5: 6 this:7!=other:5: 7 this:8!=other:5: 8 this:9!=other:5: 9 =1=this:-1!=other:5: -1 this:1!=other:5: 1 this:2!=other:5: 2 this:3!=other:5: 3 this:4!=other:5: 4 this:5!=other:5:


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
  //                                         Serial.print("\noperator --()\t");
  // itB = arr.begin();
  // itE =  arr.end();
  // for (; itB!=itE; --itE){
  //   Serial.printf("%d ",*(itE));       // Iterator	operator --()
  // --itE ;                              
  // }                               Serial.print("\noperator !=\t");
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