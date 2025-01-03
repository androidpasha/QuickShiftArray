#include <Arduino.h>
#include <algorithm>
#include "CyclicArray.h"

#define ARR_SIZE 10                     //Количество элементов массива. 
CyclicArray<int> arr(ARR_SIZE);         // создаем массив интов размером ARR_SIZE на куче
CyclicArray<int, ARR_SIZE> nextArr;     // создаем массив интов размером ARR_SIZE на куче
CyclicArray<int> zeroArr;               // создаем массив интов размером 0 на куче. Дальше метод zeroArr.setSize(NEW_ARR_SIZE) изменяет размер. На куче создается новый массив и в него копируеться старые элементы если были в количестве <= NEW_ARR_SIZE

template <typename T>                   //Функция выводит массив в Serial
void print(T arr){                      //передача по значению (вызываеться конструктор копии)
  for (auto &e : arr)
    Serial.printf("%d ", e);
  Serial.println();
}

template <typename T> 
void print2darr(T &arr2d){                 //передача по ссылке 
    const int rows = arr2d.size(), cols = arr2d[0].size();
    for (int i = 0; i < rows; i++) {
        Serial.printf("\n");
        for (int j = 0; j < cols; j++)
            Serial.printf("%d\t", arr2d[i][j]);
    }
}

void setup()
{
  Serial.begin(74880);
              Serial.println();
  CyclicArray<int>sortArr{-1,1,-3,3,-2,2,-5,5,-4,4};
  std::sort(sortArr.begin(), sortArr.end(), std::greater<int>());  
  print(sortArr);       
  std::sort(sortArr.begin(), sortArr.end());  
  print(sortArr); 
  
  auto ShowStr = [](auto str){Serial.printf("%s, %s %s\n",str[0], str[1], str[2]);};
  CyclicArray<const char*> str{"казнить", "нельзя", "помиловать"}; //инициализация списком
  ShowStr(str);                     //казнить, нельзя помиловать
  str >> 1;                         //сдвинуть весь массив вправо на одну позицию
  ShowStr(str);                     //помиловать, казнить нельзя

  uint32_t freeHeap;
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
  Serial.printf("После сдвига arr[0] читается из ячейки [%d] исходного массива\n", arr.getOffset()); // из ячейки [2]
             
              Serial.println("arr >> 3;");
  arr >> 3;
  // arr >> arr.getOffset();// Установить смещение в 0.
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

              Serial.println();
  print (arr);         //10 11 12 13 14 15 16 17 18 19

  arr << 5;
  for(const auto &e : arr)
    Serial.printf("%d ", e);                      //15 16 17 18 19 10 11 12 13 14
  Serial.println();

  auto showArr = [](int val){Serial.printf("%d ", val);};
  std::for_each(std::begin(arr), std::end(arr), showArr); //15 16 17 18 19 10 11 12 13 14

  auto predicate_IsEven = [](int val){return val % 2 == 0;};
  size_t evenNumbersCount = std::count_if(arr.begin(), arr.end(), predicate_IsEven);
  Serial.printf("\nЧетных чисел в массиве %d шт. Нечетных %d шт.", evenNumbersCount, arr.size()-evenNumbersCount);
  arr.setOffset(0);             // установить смещение массива в нулевую позицию (исходный массив) 
//_______________Чтение элементов массива используя инкремент и декремент___________
  Serial.print("\nЧтение элементов массива используя инкремент и декремент:\nИсходный массив:\n");
  print (arr);              //10 11 12 13 14 15 16 17 18 19
  int element;
  element = arr++;          //==10 возвратит нулевой элемент массива и переместит внутренний указатель на первый элемент
  element = arr++;          //==11 возвратит первый элемент массива и переместит внутренний указатель на второй элемент
  element = ++arr;          //==13 переместит внутренний указатель на третий элемент и возвратит третий элемент массива 
  Serial.printf("\nelement == %d, circle index = %d\n", element, arr.getCircleIdx());//element == 13, circle index = 3
  arr-- = 5000;             //Присвоит третьему элементу значение 5000  и переместит внутренний указатель на второй элемент
  arr.setCircleIdx(0);      //Устанавливаем круговую позицию на нулевой элемент. Команда возвращает значение указанного элемента
  element = arr.setCircleIdx(0); // element = 10
  print (arr);              //10 11 12 5000 14 15 16 17 18 19

  for(int i = 0; i < ARR_SIZE * 4; i++ ){ //Круговое заполнение массива 4 раза с перезаписью прежних данных. После записи последнего элемента начинаем запись с начала массива.
     arr++ = i;
  // ++arr //39 30 31 32 33 34 35 36 37 38 начинает запись в первый индекс 1->2->3...n->0->1->...
  // arr++ //30 31 32 33 34 35 36 37 38 39 начинает запись в нулевой индекс 0->1->2...n->0->...
  // --arr //39 38 37 36 35 34 33 32 31 30 начинает запись в последний индекс в обратном порядке n->n-1...0->n->...
  // arr-- //30 39 38 37 36 35 34 33 32 31 начинает запись в нуливой индекс и идет в обратном порядке 0->n->n-1...1->0...
  }
  print (arr);              //30 31 32 33 34 35 36 37 38 39
  arr.setCircleIdx(0); 
  arr << 1;
  print (arr);              //31 32 33 34 35 36 37 38 39 30
  arr++ = 310;              // нулевой элемент = 310. CircleIdx переместится на 1й єлемент с 0го
  print (arr);              //310 32 33 34 35 36 37 38 39 30
  arr >> 1;
  print (arr);              //30 310 32 33 34 35 36 37 38 39
  arr++ = 320;              // 1й єлемент = 320. CircleIdx переместится на 2й єлемент
  print (arr);              //30 320 32 33 34 35 36 37 38 39 т.к. массив сдвинулся (arr >> 1;) получаем такой результат
    {
      CyclicArray<int> arr;
      CyclicArray<int> arr2{1,2,3};
      
      Serial.printf("\n\nCyclicArray<int> arr;\narr.size()=%d\n",arr.size());
      arr.setSize(10);
      Serial.printf("arr.setSize(10);\narr.size()=%d\n",arr.size());
      print(arr);       //мусор 0 35 36 37 38 39 891336144 13048281 0 -776613731
      arr=arr2;
      Serial.printf("CyclicArray<int> arr2{1,2,3};\narr=arr2\n");
      print(arr);       //1 2 3
      Serial.printf("arr.size()=%d\n",arr.size());
      arr.setSize(5);
      Serial.printf("arr.setSize(5)\n");
      print(arr);       //1 2 3 13310425 0
      arr[3] = 4;
      arr.setSize(4);
      Serial.printf("arr[3] = 4;\narr.setSize(4)\n");
      print(arr);       //1 2 3 4
    }
    {//2D array 
      CyclicArray<int> rows1{1,2,3};
      CyclicArray<int> rows2{4,5,6};
      CyclicArray<int> rows3{7,8,9};

      CyclicArray<CyclicArray<int>> arr2d{rows1, rows2, rows3 };
      //CyclicArray<CyclicArray<int>> arr{ {1,2,3}, {4,5,6}, {7,8,9} }; // Тот же результат
      // arr2d[2][0] = 0; //Работа с элементами
      print2darr(arr2d);/* result:
                      1       2       3
                      4       5       6
                      7       8       9  */
      Serial.println();
    }   
    
    {//2D array
      CyclicArray<CyclicArray<int, 3>, 4> arr2d; //  3 столбца 4 строки
      const int rows = arr2d.size(), cols = arr2d[0].size();
      for (int i{}; i < rows; ++i)
          for (int j{}; j < cols; ++j)
              arr2d[i][j] = i * cols + j + 1;
      // for (int i{}; i < rows; ++i) 
      //     arr2d[i] >> 1;  // сдвиг столбцов вправо
      print2darr(arr2d);/*  1       2       3
                            4       5       6
                            7       8       9
                            10      11      12*/
    }
    {//Конструктор переноса и operator= по переносу
      CyclicArray<int> tmpArr{1, 2, 3};
      CyclicArray<int> tmpArr2{1, 2, 3};
      CyclicArray<int> moveArr = std::move(tmpArr); // Вызов конструктора переноса. tmpArr переносится в moveArr. tmpArr теперь содержит nullptr
      moveArr = std::move(tmpArr2); // Вызов operator= по переносу
      auto func = [](){CyclicArray<int> temp{9,8,7}; return temp;};
      CyclicArray<int> newArr = func();// сразу создаться newArr2 в функции без явных вызовов доп. конструкторов (оптимизация скорее всего inline)
      newArr = func(); // Вызов operator= по переносу, затем деструктор для временного объекта в котором после переноса nullptr.
    }

    {//Константная ссылка на CyclicArray
      const CyclicArray<int>& constArr = arr; //тип можно писать и так const auto&
      Serial.printf("\n%d",constArr[0]); //30
     // constArr[0] = 5; // Ошибка на этапе компиляции
    }

delay(1000000);
}

void loop(){ yield(); }