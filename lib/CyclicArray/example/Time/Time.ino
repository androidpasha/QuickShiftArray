#include <Arduino.h>
#include "CyclicArray.h"
#include <array>
#include <vector>
#define ARR_SIZE 100            //Количество ячеек массива
#define ITERATIONS 100000       // Количество повторений при замере времени

    CyclicArray<int> cyclicArray(ARR_SIZE);
    std::array<int, ARR_SIZE> stdArr;
    std::vector<int> vector(ARR_SIZE);
    int classicArr[ARR_SIZE];

    uint32_t startTime;
    size_t c;

template<typename T>
inline uint32_t shiftArr(T __first, T __middle, T __last){
  c = ITERATIONS;
  startTime = millis();
  while (c--)
      std::rotate(__first, __middle, __last);
  ESP.wdtFeed();
  return millis()-startTime;
}

template<typename T>
inline uint32_t push_back(T __first, T __middle, T __last){
  c = ITERATIONS;
  startTime = millis();
  while (c--){
      *__first = 0;
      std::rotate(__first, __middle, __last);
  }
  ESP.wdtFeed();
  return millis()-startTime;
}

template <typename T>
inline uint32_t forLoop(T &arr){
  ESP.wdtFeed();
  int c = ITERATIONS;
  uint32_t startTime = millis();
  while (c--)
    for(size_t i = 0; i < ARR_SIZE; i++)
      arr[i] = 0;
  return millis()-startTime;
}

template <typename T>
inline uint32_t rangeBasedForLoop(T &arr){
  ESP.wdtFeed();
  size_t c = ITERATIONS;
  uint32_t startTime = millis();
  while (c--)
    for(auto &e : arr)
      e = 0;
  return millis()-startTime;
}

template <typename T>
inline uint32_t iteratorForLoop(T &arr){
  ESP.wdtFeed();
  int c = ITERATIONS;
  uint32_t startTime = millis();
  while (c--)
      for (auto it =  arr.begin(); it != arr.end(); ++it)
        *it = 0;
  return millis()-startTime;
}
template <typename T>
inline volatile uint32_t circleFillArr(T &arr){
  ESP.wdtFeed();
  int c = ITERATIONS;
  uint32_t startTime = millis();
  while (c--)
    for(auto &e : arr)
      e = c;
  return millis()-startTime;
}

void setup()
{
  ESP.wdtDisable();
  Serial.begin(74880);


  Serial.printf("\nРазмер cyclicArray %d, stdArr %d, vector %d", sizeof(cyclicArray), sizeof(stdArr),sizeof(vector));
  Serial.printf("\nРазмер масивов по %d ячеек;\nКоличество повторений при замере времени %d раз.\n\n", ARR_SIZE, ITERATIONS);

  Serial.println("\nВремя сдвига:\n");
// ____________________Сдвиг массивов
  c = ITERATIONS;
  startTime = millis();
  while (c--)
         cyclicArray << 1;
  Serial.printf("cyclicArray: \t\t%lu мс\n", millis() - startTime);

  Serial.printf("classicArr: \t\t%u мс\n", shiftArr(classicArr, classicArr + 1, classicArr + ARR_SIZE)); 
  Serial.printf("std::array: \t\t%u мс\n", shiftArr(stdArr.begin(), stdArr.begin() + 1,  stdArr.end())); 
  Serial.printf("std::vector: \t\t%u мс\n", shiftArr(vector.begin(), vector.begin() + 1, vector.end())); 


// ____________________Заполнение__________________________
  Serial.println("\nЗаполнение forLoop:\n");

  Serial.printf("cyclicArray: \t\t%u мс\n", forLoop(cyclicArray));
  Serial.printf("std::array: \t\t%u мс\n", forLoop(stdArr));
  Serial.printf("std::vector: \t\t%u мс\n", forLoop(vector));
  Serial.printf("classicArr: \t\t%u мс\n", forLoop(classicArr));

 
  Serial.println("\nЗаполнение range-based for loop:\n");

  Serial.printf("cyclicArray: \t\t%u мс\n", rangeBasedForLoop(cyclicArray));
  Serial.printf("std::array: \t\t%u мс\n", rangeBasedForLoop(stdArr));
  Serial.printf("std::vector: \t\t%u мс\n", rangeBasedForLoop(vector));
  Serial.printf("classicArr: \t\t%u мс\n", rangeBasedForLoop(classicArr));
  
  Serial.println("\nЗаполнение for loop через итераторы:\n");
  
  Serial.printf("cyclicArray: \t\t%u мс\n", iteratorForLoop(cyclicArray));
  Serial.printf("std::array: \t\t%u мс\n", iteratorForLoop(stdArr));
  Serial.printf("std::vector: \t\t%u мс\n", iteratorForLoop(vector));

// ____________________Круговое заполнение__________________________
 Serial.println("\nКруговое циклическое заполнение массива:\n");
  ESP.wdtFeed();
  startTime = millis();
    for (int i = 0; i < ITERATIONS; i++) 
    cyclicArray++ = i;
  Serial.printf("cyclicArray \t\t%lu мс\n", millis() - startTime);

  Serial.printf("std::array: \t\t%u мс\n", circleFillArr(stdArr));
  Serial.printf("std::vector: \t\t%u мс\n", circleFillArr(vector));
  Serial.printf("classicArr: \t\t%u мс\n", circleFillArr(classicArr));

//____________Вставка в конец со сдвигом остальных элементов влево____________
 Serial.println("\nВставка в конец со сдвигом остальных элементов влево:\n");
  c = ITERATIONS;
  startTime = millis();
  while (c--){
      cyclicArray << 1;
      cyclicArray[ARR_SIZE-1] = 0;
  }
  Serial.printf("cyclicArray: \t\t%lu мс\n", millis() - startTime);

  Serial.printf("classicArr: \t\t%u мс\n", push_back(classicArr, classicArr + 1, classicArr + ARR_SIZE)); 
  Serial.printf("std::array: \t\t%u мс\n", push_back(stdArr.begin(), stdArr.begin() + 1,  stdArr.end())); 
  Serial.printf("std::vector: \t\t%u мс\n", push_back(vector.begin(), vector.begin() + 1, vector.end())); 



 }
void loop() {  ESP.wdtFeed(); }

/*
Размер cyclicArray 20, stdArr 400, vector 12
Размер масивов по 100 ячеек;
Количество повторений при замере времени 100000 раз.


Время сдвига:

cyclicArray:            10 мс
classicArr:             511 мс
std::array:             511 мс
std::vector:            516 мс

Заполнение forLoop:

cyclicArray:            3630 мс
std::array:             312 мс
std::vector:            308 мс
classicArr:             313 мс

Заполнение range-based for loop:

cyclicArray:            4938 мс
std::array:             314 мс
std::vector:            312 мс
classicArr:             314 мс

Заполнение for loop через итераторы:

cyclicArray:            7685 мс
std::array:             313 мс
std::vector:            307 мс

Круговое циклическое заполнение массива:

cyclicArray             64 мс
std::array:             1630 мс
std::vector:            1008 мс
classicArr:             1631 мс

Вставка в конец со сдвигом остальных элементов влево:

cyclicArray:            51 мс
classicArr:             516 мс
std::array:             516 мс
std::vector:            524 мс
*/