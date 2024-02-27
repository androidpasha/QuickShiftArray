#include <Arduino.h>
#include "CyclicArray.h"
#define ARR_SIZE 10                     //Количество элементов массива

CyclicArray<int> arr(ARR_SIZE);         // создаем массив интов размером ARR_SIZE

template <typename T>                   //Функция выводит массив в Serial
void print(T arr){                      //передача в функцию по значению работает
  for (size_t i = 0; i < arr.size(); i++)
    Serial.printf("%d ", arr[i]);
  Serial.println();
}

void setup()
{
  Serial.begin(74880);
  Serial.println();
  uint32_t freeHeap;

  CyclicArray<int> arr2 {1,2,3,4,5}; //инициализация списком
  print(arr2);        //1 2 3 4 5
              Serial.println("arr2 >> 1;");
  arr2 >> 1;          // сместит весь массив вправо на одну позицию
  print(arr2);        //5 1 2 3 4


  CyclicArray<int> arr3 = {1,2,3};  //инициализация списком через =
  print(arr3);        //1 2 3
  
  { CyclicArray<int> arr(10000);    //10000 по 4 байта = 40 000 байт
    freeHeap = ESP.getFreeHeap();
  } //Работа деструктора c освобождением памяти
  Serial.printf("\nОсвободилось %d байт на куче.\n", ESP.getFreeHeap() - freeHeap);       //Освободилось 40008 байт на куче.
  Serial.printf("sizeof(arr) = %u байт на стеке занимает экземпляр CyclicArray\n", sizeof(arr));   //sizeof(arr) = 20 байт на стеке занимает экземпляр CyclicArray
  Serial.printf("arr.size() = %u элементов в масиве\n", arr.size());                      //arr.size() = 10 элементов в масиве

  for (byte i = 0; i < arr.size(); i++)
      arr[i] = i;

  print (arr);        //0 1 2 3 4 5 6 7 8 9
              Serial.println("arr << 2;");
  arr << 2;
  print (arr);        //2 3 4 5 6 7 8 9 0 1
              Serial.println("arr >> 3;");
  arr >> 3;
  print (arr);        //9 0 1 2 3 4 5 6 7 8
              Serial.println("arr << 1;");
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
  print (arr);              //10 11 12 13 14 15 16 17 18 19

  arr << 5;
  for(const auto &e : arr)
    Serial.printf("%d ",e);     //15 16 17 18 19 10 11 12 13 14
  Serial.println();

  for (auto it = arr.begin(); it != arr.end(); ++it)
    Serial.printf("%d ",*it);   //15 16 17 18 19 10 11 12 13 14
  arr >> 5;
//_______________Чтение элементов массива используя инкремент и декремент___________
  Serial.print("\nЧтение элементов массива используя инкремент и декремент:");
  int element;
  element = arr++;          //возвратит нулевой элемент массива и переместит внутренний указатель на первый элемент
  element = arr++;          //возвратит первый элемент массива и переместит внутренний указатель на второй элемент
  element = ++arr;          //переместит внутренний указатель на третий элемент и возвратит третий элемент массива 
  Serial.printf("\nelement == %d, circle index = %d\n", element, arr.getCircleIdx());//element == 13, circle index = 3
  arr-- = 5000;             //Присвоит третьему элементу значение 5000  и переместит внутренний указатель на второй элемент
  arr.setCircleIdx(0);      //Устанавливаем круговую позицию на нулевой элемент. Команда возвращает значение указанного элемента
  element = arr.setCircleIdx(0); // element = 10
  print (arr);              //10 11 12 5000 14 15 16 17 18 19

  for(int i = 0; i<ARR_SIZE*4; i++ ){ //Круговое заполнение массива 4 раза с перезаписью прежних данных. Последний элемент перескочит на первую позицию.
     arr++ = i;
  // ++arr //39 30 31 32 33 34 35 36 37 38 начинает запись в первый индекс 1->2->3...n->0->1->...
  // arr++ //30 31 32 33 34 35 36 37 38 39 начинает запись в нулевой индекс 0->1->2...n->0->...
  // --arr //39 38 37 36 35 34 33 32 31 30 начинает запись в последний индекс в обратном порядке n->n-1...0->n->...
  // arr-- //30 39 38 37 36 35 34 33 32 31 начинает запись в нуливой индекс и идет в обратном порядке 0->n->n-1...1->0...
  }
  print (arr);              //30 31 32 33 34 35 36 37 38 39
}

void loop(){ yield(); }