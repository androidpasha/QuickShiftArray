#define UART_DEBUG
#include "debug.h"
#include <Arduino.h>
#include "CyclicArray.h"
#include <array>
#define ARR_SIZE 161            //Количество ячеек массива
#define ITERATIONS 10000

CyclicArray<int> cyclicArray(ARR_SIZE);
int measure = 0;                // значение нового измерения
int MAX = 0;
uint32_t startTime = 0;

int DATA_old[ARR_SIZE];
int SAVE_MAX = 0;
byte NUMBER = 0;

template <typename T>
void print(T &arr)
{
  for (size_t i = 0; i < ARR_SIZE; i++)
    Serial.printf("%lu ", arr[i]);
  Serial.println();
}

void setup()
{
  Serial.begin(74880);
  Serial.println();



//___Новый алгоритм. При каждом замере сдвигает массив влево. Новое значение пишет в конец массива____
  for (size_t i = 0; i < cyclicArray.size(); i++) // обнуляем массив
    cyclicArray[i] = 0;

  size_t c = ITERATIONS;                  // количество замеров
  startTime = millis();
  while (c--)
  {
    measure = random(0, 10000);     // Результат очередного измерения

    if (measure >= MAX)             // Если намеряли больше максимума
      MAX = measure;                // Сохранили новый максимум
    else if (cyclicArray[0] == MAX){ // Намеряли немного. Если отбрасываемое значение было максимумом то ищем новый
    // Serial.println("Search max...");
      for (size_t i = 1; i < cyclicArray.size(); i++) // т.к. нулевой элемент откинули и он был макс то ищем со второго(1)
        MAX = std::max(MAX, cyclicArray[i]);
    }

    cyclicArray.push_back(measure);  // сдвигаем массив влево и в конец добавляем новое значение. Левое значение удаляеться
    // Serial.printf("max=%lu", MAX);
    // print(DATA);
  }
  Serial.printf("Время нового алгоритма на 10000 замеров = %lu мс\n", millis() - startTime);


  // ________________QuickShiftArray для швабры___________________
  for (size_t i = 0; i < cyclicArray.size(); i++) // обнуляем массив
    cyclicArray[i] = 0;
  SAVE_MAX = 0;
  NUMBER = 0;


  c = ITERATIONS; // количество замеров
  startTime = millis();
  while (c--)
  {
    measure = random(0, 10000); // Результат очередного измерения

    if (measure >= MAX) // Если намеряли больше максимума
      MAX = measure;    // Сохранили новый максимум
    else if (cyclicArray[NUMBER] == MAX){ // Намеряли немного. Если отбрасываемое значение было максимумом то ищем новый
      // Serial.println("Search max...");
      for (size_t i = 0; i < cyclicArray.size(); i++){ 
        if (i == NUMBER) continue; // т.к.  элемент[NUMBER] откинули и он был макс то его не берем в расчет
        MAX = std::max(MAX, cyclicArray[i]);
      }
    }

    cyclicArray[NUMBER] = measure; // Результат очередного измерения

    if (NUMBER == 160)
    {
      cyclicArray << 1; // Сдвигаем масив влево на 1
      --NUMBER;
    }
    ++NUMBER;
    // Serial.printf("max=%lu", MAX);
  }
  Serial.printf("Время аналогичного нового алгоритма на 10000 замеров = %lu мс\n", millis() - startTime);



  // ______________________Прежний способ массив стандартный________________________
  c = ITERATIONS; // количество замеров
  SAVE_MAX = 0;
  NUMBER = 0;

  startTime = millis();
  while (c--)
  {
    DATA_old[NUMBER] = random(0, 10000); // Результат очередного измерения

    if (NUMBER == 160)
    {
      for (byte i = 0; i < NUMBER; ++i)
          DATA_old[i] = DATA_old[i + 1];
      --NUMBER;
    }
    ++NUMBER;
    MAX = 0;
    for (byte i = 0; i < NUMBER; ++i)
      MAX = max(MAX, DATA_old[i]); // Получаем максимальное значение массива
    // Serial.printf("max=%lu", MAX);
  }
  Serial.printf("Время старого алгоритма на 10000 замеров = %lu мс\n", millis() - startTime);
}

void loop() { yield();}