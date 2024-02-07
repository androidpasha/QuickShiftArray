#include <Arduino.h>
#include "QuickShiftArray.h"
#include <array>
#include <vector>
#define ARR_SIZE 100            //Количество ячеек массива
#define ITERATIONS 500000       // Количество повторений при замере времени

    QuickShiftArray<int> quickShiftArray(ARR_SIZE);
    std::array<int, ARR_SIZE> stdArr;
    std::vector<int> vector(ARR_SIZE);
    int classicArr[ARR_SIZE];

    uint32_t startTime;
    int c;

template <typename T>
inline uint32_t forLoop(T &arr){
  ESP.wdtFeed();
  int c = ITERATIONS;
  uint32_t startTime = millis();
  while (c--)
    for(int i = 0; i < ARR_SIZE; i++)
      arr[i] = 0;
  return millis()-startTime;
}

template <typename T>
inline uint32_t rangeBasedForLoop(T &arr){
  ESP.wdtFeed();
  int c = ITERATIONS;
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


void setup()
{
  ESP.wdtDisable();
  Serial.begin(74880);


  Serial.printf("\nРазмер quickShiftArray %d, stdArr %d, vector %d", sizeof(quickShiftArray), sizeof(stdArr),sizeof(vector));
  Serial.printf("\nРазмер масивов по %d ячеек;\nКоличество повторений при замере времени %d раз.\n\n", ARR_SIZE, ITERATIONS);

Serial.println("\nВремя сдвига:\n");
// ____________________Сдвиг QuickShiftArray________________________
  c = ITERATIONS;
  startTime = millis();
  while (c--)
         quickShiftArray << 1;
  Serial.printf("quickShiftArray \t%lu мс\n", millis() - startTime);

// ____________________Сдвиг класического массива________________________
  c = ITERATIONS;
  startTime = millis();
  while (c--)
  {
    int first = classicArr[0];
      for (int i = 0; i < ARR_SIZE-1; ++i)
        classicArr[i] = classicArr[i+1];
    classicArr[ARR_SIZE-1] = first;
  }
  Serial.printf("classicArr \t\t%lu мс\n", millis() - startTime);

// ____________________ Сдвиг std::arr__________________________
  c = ITERATIONS;
  startTime = millis();
  while (c--)
      std::rotate(stdArr.begin(), stdArr.begin() + 1, stdArr.end());
  Serial.printf("std::array: \t\t%lu мс\n", millis() - startTime);
  ESP.wdtFeed();
  // ____________________ Сдвиг vector__________________________
  c = ITERATIONS; // количество замеров
  startTime = millis();
  while (c--)
      std::rotate(vector.begin(), vector.begin() + 1, vector.end());
  Serial.printf("std::vector: \t\t%lu мс\n", millis() - startTime);
  ESP.wdtFeed();

// ____________________Заполнение__________________________
  Serial.println("\nЗаполнение forLoop:\n");
  
  Serial.printf("quickShiftArray: \t%u мс\n", forLoop(quickShiftArray));
  Serial.printf("std::array: \t\t%u мс\n", forLoop(stdArr));
  Serial.printf("std::vector: \t\t%u мс\n", forLoop(vector));
  Serial.printf("classicArr: \t\t%u мс\n", forLoop(classicArr));

 
  Serial.println("\nЗаполнение range-based for loop:\n");

  Serial.printf("quickShiftArray: \t%u мс\n", rangeBasedForLoop(quickShiftArray));
  Serial.printf("std::array: \t\t%u мс\n", rangeBasedForLoop(stdArr));
  Serial.printf("std::vector: \t\t%u мс\n", rangeBasedForLoop(vector));
  Serial.printf("classicArr: \t\t%u мс\n", rangeBasedForLoop(classicArr));
  
  Serial.println("\nЗаполнение for loop через итераторы:\n");
  
  Serial.printf("quickShiftArray: \t%u мс\n", iteratorForLoop(quickShiftArray));
  Serial.printf("std::array: \t\t%u мс\n", iteratorForLoop(stdArr));
  Serial.printf("std::vector: \t\t%u мс\n", iteratorForLoop(vector));
 }
void loop() {  ESP.wdtFeed(); }