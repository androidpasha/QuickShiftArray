/* Библиотека сдвигового массива.
Основное преимущество над классическим массивом скорость сдвига O(1) и составляет 
примерно 0,1 мкс для ESP8266. Для сравнения сдвиг класического массива из 
100 элементов занимает примерно 4,5 мкс.
(например был массив 0123 при сдвиге влево на 1 вышло 1230)
Присутствует функция push_back и push_front для добавления новых данных в края массива,
при этом массив сдвигается влево(вправо) на один элемент. противоположный вставке
элемент исчезает.
---
При создании экземпляра на куче выделяется память под новый массив.
Библиотека хранит номер элемента, с которого при вызове оператора [index]
начинается отсчет "нулевого элемента для пользователя". При сдвиге массива
изменяется номер элемента начала отсчета вместо перезаписи всего массива.
---
Присутствует конструктор копии для передачи массива по значению.
При удалении массива (окончание блока кода {}) вызывается деструктор
который освобождает память на куче.
Класс QuickShiftArray шаблонный и может создавать массивы с любыми типами данных:
QuickShiftArray<ТИП_ДАННЫХ> НАЗВАНИЕ_МАССИВА(КОЛИЧЕСТВО_ЯЧЕЕК);
Обращение к ячейкам как в обычном массиве: НАЗВАНИЕ_МАССИВА[ИНДЕКС];
Добавление новых данных в конец со сдвигом массива влево:
НАЗВАНИЕ_МАССИВА.push_back(НОВОЕ_ЗНАЧЕНИЕ);
Добавление новых данных в начало со сдвигом массива вправо:
НАЗВАНИЕ_МАССИВА.push_front(НОВОЕ_ЗНАЧЕНИЕ);
---
Сдвиг массива: НАЗВАНИЕ_МАССИВА << КОЛИЧЕСТВО_ШАГОВ_СДВИГА;
НАЗВАНИЕ_МАССИВА >> КОЛИЧЕСТВО_ШАГОВ_СДВИГА;
Количество ячеек в массиве size_t: НАЗВАНИЕ_МАССИВА.size();
Пройтись по всем элементам: for(size_t i = 0; i < НАЗВАНИЕ_МАССИВА.size(); i++){}
---
Недостатки:
Скорость доступа к элементам в  дольше чем у классического массива из-за подсчета индексов.
Несовместим c алгоритмами stl.
*/


#pragma once
#include <iterator>
#include <initializer_list>
#ifdef ARDUINO
#include<Arduino.h>
#endif
//#define VALIDATION // проверка на обращение к ячейкам вне массива и сдвигам более длинны массива. Немного замедляет работу. !!! Не работает для итераторов!


template<typename T>
class QuickShiftArray
{
public:
    friend struct Iterator;
	struct Iterator : public std::iterator<std::random_access_iterator_tag, T> {
		explicit Iterator(T* beginPtr, T* shiftPtr, T* endPtr) :
		size(endPtr-beginPtr), endPtr(endPtr), shiftPtr(shiftPtr){} //Конструктор. Порядок членов класса должен соответствовать порядку списка инициализации 
		Iterator& operator++() {++shiftPtr; return *this;}
		Iterator& operator--() {--shiftPtr; return *this;}
		bool operator==(const Iterator& other) const { return shiftPtr == other.shiftPtr; }
		bool operator!=(const Iterator& other) const { return !(*this == other); } //Перегрузка оператора, которую использует цикл для условия
		bool operator<(const Iterator& other) const { return !(*this < other); } //Перегрузка оператора, которую использует цикл для условия
		T& operator*() { return (shiftPtr < endPtr) ? *shiftPtr : *(shiftPtr - size); }
/*		T& operator[](size_t index) const {
        if(shiftPtr + index < endPtr)
			return shiftPtr[index];
		else	
			return (shiftPtr - size)[index];    }
*/
		private:
		size_t size; T* endPtr; T* shiftPtr; 
	};

	QuickShiftArray(size_t size);						//конструктор. создает на куче массив
	QuickShiftArray(std::initializer_list<T> initList); //конструктор инициализации списком
	QuickShiftArray(const QuickShiftArray& other);		//конструктор копирования для передачи по значению
	inline T& operator [] (size_t index);						//возвращает указатель на элементу массива с номером index
	void operator >> (size_t shift);					//сдвигает массив вправо (меняется индекс отсчета)
	void operator << (size_t shift);					//сдвигает массив влево
	void push_back(const T &newVal);					//сдвигает массив влево на одну позицию и добавляет в последнюю ячейку новые данные
	void push_front(const T &newVal);					//сдвигает массив вправо на одну позицию и добавляет в начальную ячейку новые данные
    void setShift(size_t shift){ piterator = arr + shift;}
	size_t getShift(){return piterator - pbegin;}
	size_t size() const;								//возвращает количество элементов массива
	Iterator begin() const { return Iterator(arr,  piterator, pend); } // 
    Iterator end() const { return Iterator(arr, pend, pend); }
	~QuickShiftArray();									//удаляет arr из кучи

private:
	//size_t iterator = 0;								//хранить текущее начальное положение массива 
	size_t _size = 0;									//хранит количество элементов массива
	T* arr = nullptr;									//указатель на созданный массив на куче
	T* piterator = nullptr;								//хранить текущее начальное положение массива 
	T* pbegin = nullptr;
	T* pend = nullptr;
};

template<typename T>
inline QuickShiftArray<T>::QuickShiftArray(size_t size) : _size(size) {
	arr = new T[_size];
	piterator = arr;
	pbegin = arr;
	pend = pbegin + _size;
}

template<typename T>
inline QuickShiftArray<T>::QuickShiftArray(std::initializer_list<T> initList){
		_size = initList.size();
		arr = new T[_size];
		piterator = arr;
		pbegin = arr;
		pend = pbegin + _size;
		size_t i = 0;
		for (T e : initList)
			arr[i++] = e;
}

template<typename T>
QuickShiftArray<T>::QuickShiftArray(const QuickShiftArray& other)
					:_size(other._size), piterator(other.piterator),
					pbegin(other.pbegin)  {
	arr = new T[_size];
	std::copy(other.arr, other.arr + _size, arr);
    piterator = arr + (piterator - pbegin);
	pbegin = arr;
	pend = pbegin + _size;
}

template<typename T>
inline T& QuickShiftArray<T>::operator[](size_t index){
	#ifdef VALIDATION
	   if (index > _size) index %= _size;
    #endif
    //return begin()[index];
	
	if (index + piterator < pend)
		return piterator[index];
	else
		return piterator[index - _size];
}

 template<typename T>
 void QuickShiftArray<T>::operator << (size_t shift){
	#ifdef VALIDATION
	  if (shift > _size) shift %= _size;
    #endif

	piterator += shift; 
	if (piterator >= pend)
		piterator -= _size;
}

template<typename T>
void QuickShiftArray<T>::operator >> (size_t shift){
	#ifdef VALIDATION
		if (shift > _size) shift %= _size;
    #endif
		
	piterator -= shift;
	if (piterator < pend)
		piterator += _size;
}

template<typename T>
void QuickShiftArray<T>::push_back(const T &newVal){
	(*this)[0] = newVal;
	(*this) << 1;
}

template<typename T>
void QuickShiftArray<T>::push_front(const T &newVal){
	 (*this) >> 1;
	 (*this)[0] = newVal;
}

template<typename T>
size_t QuickShiftArray<T>::size() const{
	return _size;
}

template<typename T>
QuickShiftArray<T>::~QuickShiftArray() {
	delete[] arr;
//	arr = nullptr;
//	arrIt = nullptr;
}