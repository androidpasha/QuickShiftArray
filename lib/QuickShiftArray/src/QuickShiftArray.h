/* Библиотека сдвигового массива.
Основное преимущество над классическим массивом скорость сдвига в 50 раз быстрее.
(например был массив 0123 при сдвиге влево на 1 вышло 1230)
Присутствует функция push_back и push_front для добавления новых данных в края массива,
при этом массив сдвигается влево(вправо) на один элемент. противоположный вставке
элемент исчезает.

При создании экземпляра на куче выделяется память под новый массив.
Библиотека хранит номер элемента, с которого при вызове оператора [index]
начинается отсчет "нулевого элемента для пользователя". При сдвиге массива
изменяется номер элемента начала отсчета вместо перезаписи всего массива.

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

Сдвиг массива: НАЗВАНИЕ_МАССИВА << КОЛИЧЕСТВО_ШАГОВ_СДВИГА;
НАЗВАНИЕ_МАССИВА >> КОЛИЧЕСТВО_ШАГОВ_СДВИГА;
Количество ячеек в массиве size_t: НАЗВАНИЕ_МАССИВА.size();
Пройтись по всем элементам: for(size_t i = 0; i < НАЗВАНИЕ_МАССИВА.size(); i++){}

Недостатки:
Скорость доступа к элементам в 2 раза дольше чем у классического массива из-за подсчета индексов.
С QuickShiftArray не работает цикл по диапазону for(auto e: QuickShiftArray){} т.к. в реальности
доступ к ячейкам идет не последовательно и в крайнем индексе адрес следующей ячейки
начинается с противоположной стороны.
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
	struct Iterator : public std::iterator<std::random_access_iterator_tag, T> {
		explicit Iterator(T* ptr, size_t shift, size_t Size=0) : Size(Size), endPtr(ptr + Size), shiftPtr(ptr + shift){} //Конструктор. Порядок членов класса должен соответствовать порядку списка инициализации 
		Iterator& operator++() {++shiftPtr; return *this;}
		Iterator& operator--() {--shiftPtr; return *this;}
		bool operator==(const Iterator& other) const { return shiftPtr == other.shiftPtr; }
		bool operator!=(const Iterator& other) const { return !(*this == other); }
		T& operator*() { return (shiftPtr < endPtr) ? *shiftPtr : *(shiftPtr - Size); }
		private:
		size_t Size; T* endPtr; T* shiftPtr; 
	};

	QuickShiftArray(size_t size);						//конструктор. создает на куче массив
	QuickShiftArray(std::initializer_list<T> initList); //конструктор инициализации списком
	QuickShiftArray(const QuickShiftArray& other);		//конструктор копирования для передачи по значению
	T& operator [] (size_t index);						//возвращает указатель на элементу массива с номером index
	void operator >> (size_t shift);					//сдвигает массив вправо (меняется индекс отсчета)
	void operator << (size_t shift);					//сдвигает массив влево
	void push_back(const T &newVal);					//сдвигает массив влево на одну позицию и добавляет в последнюю ячейку новые данные
	void push_front(const T &newVal);					//сдвигает массив вправо на одну позицию и добавляет в начальную ячейку новые данные
    void setShift(size_t shift){iterator = shift;}
	size_t getShift(){return iterator;}
	size_t size() const;								//возвращает количество элементов массива
	Iterator begin() const { return Iterator(arr, iterator, _size); }
    Iterator end() const { return Iterator(arr + _size, iterator); }
	~QuickShiftArray();									//удаляет arr из кучи

private:
	size_t iterator = 0;								//хранить текущее начальное положение массива 
	size_t _size = 0;									//хранит количество элементов массива
	T* arr = nullptr;									//указатель на созданный массив на куче
};

template<typename T>
inline QuickShiftArray<T>::QuickShiftArray(size_t size) : _size(size) {
	arr = new T[_size];
}

template<typename T>
inline QuickShiftArray<T>::QuickShiftArray(std::initializer_list<T> initList){
		_size = initList.size();
		arr = new T[_size];
		size_t i = 0;
		for (T e : initList)
			arr[i++] = e;
}

template<typename T>
T& QuickShiftArray<T>::operator[](size_t index){
	#ifdef VALIDATION
	   if (index > _size) index %= _size;
    #endif
	
	if (index + iterator < _size)
		return arr[index + iterator];
	else
		return arr[index + iterator - _size];
}

template<typename T>
QuickShiftArray<T>::QuickShiftArray(const QuickShiftArray& other)
					: iterator(other.iterator), _size(other._size) {
	arr = new T[_size];
	std::copy(other.arr, other.arr + _size, arr);
}

template<typename T>
QuickShiftArray<T>::~QuickShiftArray() {
	delete[] arr;
	arr = nullptr;
}

 template<typename T>
 void QuickShiftArray<T>::operator << (size_t shift){
	#ifdef VALIDATION
	  if (shift > _size) shift %= _size;
    #endif
		
	iterator += shift;
	if (iterator >= _size)
		iterator -= _size;
}

template<typename T>
void QuickShiftArray<T>::operator >> (size_t shift){
	#ifdef VALIDATION
		if (shift > _size) shift %= _size;
    #endif
		
	if (iterator < shift)
		iterator += _size - shift;
	else
		iterator -= shift;
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