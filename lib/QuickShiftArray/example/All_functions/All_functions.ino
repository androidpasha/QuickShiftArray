#include <Arduino.h>
#include "QuickShiftArray.h"
#define ARR_SIZE 10                 //Количество ячеек массива


QuickShiftArray<int> arr(ARR_SIZE); // создаем массив интов размером ARR_SIZE

template <typename T>
void print(T arr)                   // передача в функцию по значению
{
  for (size_t i = 0; i < arr.size(); i++)
   Serial.printf("%d ", arr[i]);
  Serial.println();
}

void setup()
{
  Serial.begin(74880);
  Serial.println();
  {
    QuickShiftArray<int> arr2 {1,2,3,4,5};    //инициализация списком
    print(arr2);        //1 2 3 4 5
    arr2 >> 1;
    print(arr2);        //1 2 3 4 5


    QuickShiftArray<int> arr3 = {1,2,3};  //инициализация списком
    print(arr3);        //1 2 3
  }
  for (byte i = 0; i < arr.size(); i++)
      arr[i] = i;

  print (arr);        //0 1 2 3 4 5 6 7 8 9
  arr << 2;
  print (arr);        //2 3 4 5 6 7 8 9 0 1
  arr >> 3;
  print (arr);        //9 0 1 2 3 4 5 6 7 8
  Serial.printf("arr.size() = %u\n", arr.size());  //arr.size() = 10
  arr << 1;
  print (arr);        //0 1 2 3 4 5 6 7 8 9
  
  arr[5]=500;
  print (arr);        //0 1 2 3 4 500 6 7 8 9

  arr.push_front(-100);
  print (arr);        //-100 0 1 2 3 4 500 6 7 8
  arr.push_back(900);
  print (arr);        //0 1 2 3 4 500 6 7 8 900
  
  int i = 10;
  for (auto it = arr.begin(); it != arr.end(); ++it)
    *it = i++;

  for(const auto &e : arr)
    Serial.printf("%d ",e); //10 11 12 13 14 15 16 17 18 19

}

void loop(){ yield(); }