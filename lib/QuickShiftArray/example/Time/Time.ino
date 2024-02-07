#include <Arduino.h>
#include "QuickShiftArray.h"
#include <array>
#include <vector>
#define ARR_SIZE 50            //Количество ячеек массива
#define ITERATIONS 500000       // Количество повторений при замере времени

void setup()
{
  ESP.wdtDisable();
  Serial.begin(74880);
  QuickShiftArray<int> quickShiftArray(ARR_SIZE);
  std::array<int, ARR_SIZE> stdArr = {0};
  std::vector<int> vector(ARR_SIZE, 0);
  volatile int classicArr[ARR_SIZE];
  uint32_t startTime;
  int c;
Serial.printf("\nРазмер quickShiftArray %d, stdArr %d, vector %d", sizeof(quickShiftArray), sizeof(stdArr),sizeof(vector));

  Serial.printf("\nРазмер масивов по %d ячеек;\nКоличество повторений при замере времени %d раз.\n\n", ARR_SIZE, ITERATIONS);


// ____________________Сдвиг QuickShiftArray________________________
  c = ITERATIONS;
  startTime = millis();
  while (c--)
  {
         quickShiftArray << 1;
  }
  Serial.printf("Время сдвига QuickShiftArray: \t%lu мс\n", millis() - startTime);

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
  Serial.printf("Время сдвига classicArr: \t%lu мс\n", millis() - startTime);

// ____________________ Сдвиг std::arr__________________________
  c = ITERATIONS; // количество замеров
  startTime = millis();
  while (c--)
  {
    std::rotate(stdArr.begin(), stdArr.begin() + 1, stdArr.end());
  }
  Serial.printf("Время сдвига std::array: \t%lu мс\n", millis() - startTime);
  ESP.wdtFeed();
  // ____________________ Сдвиг vector__________________________
  c = ITERATIONS; // количество замеров
  startTime = millis();
  while (c--)
  {
    std::rotate(vector.begin(), vector.begin() + 1, vector.end());
  }
  Serial.printf("Время сдвига std::vector: \t%lu мс\n", millis() - startTime);
  ESP.wdtFeed();
// ____________________Заполнение std::arr__________________________
  c = ITERATIONS; // количество замеров
  startTime = millis();
  while (c--)
  {
      for (int i = 0; i < ARR_SIZE; i++)
        stdArr[i] = i;
  }
  Serial.printf("Время заполнения std::array: \t%lu мс\n", millis() - startTime);
  ESP.wdtFeed();
  // ____________________Заполнение std::vector__________________________
  c = ITERATIONS; // количество замеров
  startTime = millis();
  while (c--)
  {
      for (int i = 0; i < ARR_SIZE; i++)
        vector[i] = i;
  }
  Serial.printf("Время заполнения std::vector: \t%lu мс\n", millis() - startTime);
  ESP.wdtFeed();
 // ____________________Заполнение класического массива________________________
  c = ITERATIONS; // количество замеров
  startTime = millis();
  while (c--)
  {
      for (int i = 0; i < ARR_SIZE; i++)
        classicArr[i] = i;
  }
  Serial.printf("Время заполнения classicArr: \t%lu мс\n", millis() - startTime);
  ESP.wdtFeed();
// ____________________Заполнение QuickShiftArray________________________
  c = ITERATIONS;
  startTime = millis();
  while (c--)
  {
      for (int i = 0; i < ARR_SIZE; i++)
        quickShiftArray[i] = i;
  }
  Serial.printf("Время за-ния QuickShiftArray: \t%lu мс\n", millis() - startTime);
  ESP.wdtFeed();

  // ____________________Заполнение QuickShiftArray циклом по диапазону________________________
  c = ITERATIONS;
  startTime = millis();
  while (c--)
  {
      for (auto &e : quickShiftArray)
          e = 0;
  }
  Serial.printf("Время forEach QuickShiftArray: \t%lu мс\n", millis() - startTime);
  ESP.wdtFeed();


 // ____________________Заполнение QuickShiftArray через итераторы________________________
  c = ITERATIONS / 2;
  startTime = millis();
  while (c--)
  {
      for (auto it =  quickShiftArray.begin(); it != quickShiftArray.end(); ++it){
        *it = 0;
      }
  }
  Serial.printf("Время iterator Q.ShiftArray: \t%lu мс\n", (millis() - startTime)*2);
  ESP.wdtFeed();

}


void loop() {  ESP.wdtFeed(); }