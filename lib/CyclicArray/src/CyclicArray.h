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
Класс CyclicArray шаблонный и может создавать массивы с любыми типами данных:
CyclicArray<ТИП_ДАННЫХ> НАЗВАНИЕ_МАССИВА(КОЛИЧЕСТВО_ЯЧЕЕК);
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
class CyclicArray
{
public:
	class Iterator : public std::iterator<std::random_access_iterator_tag, T> {
			size_t size; T* endPtr; T* shiftPtr; //размер массива, указатели на конец и нулевую позицию, заданную пользователем
		public: 
			explicit Iterator(T* beginPtr, T* shiftPtr, T* endPtr) :
				size(endPtr-beginPtr), endPtr(endPtr), shiftPtr(shiftPtr){}
			Iterator& operator++() {++shiftPtr; return *this;}
			Iterator& operator--() {--shiftPtr; return *this;}
			bool operator==(const Iterator& other) const { return shiftPtr == other.shiftPtr; }
			bool operator!=(const Iterator& other) const { return !(*this == other); }
			//bool operator<(const Iterator& other) const { return (*this < other); }
			T& operator*() { return (shiftPtr < endPtr) ? *shiftPtr : *(shiftPtr - size); }
	};

	CyclicArray(size_t size);						//конструктор. создает на куче массив
	CyclicArray(std::initializer_list<T> initList); //конструктор инициализации списком
	CyclicArray(const CyclicArray& other);			//конструктор копирования для передачи по значению
	T& operator [] (size_t index);					//возвращает указатель на элементу массива с номером index
	void operator >> (size_t shift);				//сдвигает массив вправо (меняется индекс отсчета)
	void operator << (size_t shift);				//сдвигает массив влево
	T& operator ++ (int);							//постфиксный инкремент
	T& operator ++ ();								//префиксный инкремент
	T& operator -- (int); 							//постфиксный декремент
	T& operator -- ();								//префиксный декремент
	size_t getCircleIdx(){return circleIdx;} 		//возвращает положение кругового индекса для чтения/записи операторами ++ и --
	T& setCircleIdx(size_t idx){circleIdx = idx; return (*this)[circleIdx];}//устанавливает круговой индекс для чтения/записи операторами ++ и -- а также возвращает значение установленной позиции
	void push_back(const T &newVal);				//сдвигает массив влево на одну позицию и добавляет в последнюю ячейку новые данные
	void push_front(const T &newVal);				//сдвигает массив вправо на одну позицию и добавляет в начальную ячейку новые данные
    void setShift(size_t shift){arrPtr = beginPtr + shift;} //Сдвигает массив влево начиная с нулевого элемента без учета прежних сдвигов
	size_t getShift(){return arrPtr - beginPtr;}            //Возвращает сдвиг
	size_t size() const{return _size;};						//возвращает количество элементов массива
	Iterator begin() const { return Iterator(beginPtr, arrPtr, endPtr); } //Возвращает iterator. Необходим для работы цикла по диапазону 
    Iterator end() const { return Iterator(beginPtr, endPtr, endPtr); }
	~CyclicArray(){ delete[] beginPtr; };									

private:
	size_t _size = 0;			//хранит количество элементов массива
	T* beginPtr = nullptr;		//указатель на начало созданного массив на куче
	T* arrPtr = nullptr;		//указатель начального положение массива с учетом сдвига массива
	T* endPtr = nullptr;		//указатель на конец массива
	size_t circleIdx = 0;		//индекс для обращения к массиву через операторы ++ и --
};

template<typename T>
inline CyclicArray<T>::CyclicArray(size_t size) : _size(size) {
	beginPtr = new T[_size];
	arrPtr = beginPtr;
	endPtr = beginPtr + _size;
}

template<typename T>
inline CyclicArray<T>::CyclicArray(std::initializer_list<T> initList){
	_size = initList.size();
	beginPtr = new T[_size];
	arrPtr = beginPtr;
	endPtr = beginPtr + _size;
	std::copy(initList.begin(), initList.end(), beginPtr);
}

template<typename T>
CyclicArray<T>::CyclicArray(const CyclicArray& other)
					:_size(other._size), arrPtr(other.arrPtr)  {
	beginPtr = new T[_size];
	std::copy(other.beginPtr, other.beginPtr + _size, beginPtr);
    arrPtr = beginPtr + (other.arrPtr-other.beginPtr);
	endPtr = beginPtr + _size;
}

template<typename T>
inline T& CyclicArray<T>::operator[](size_t index){
	#ifdef VALIDATION
	   if (index > _size) index %= _size;
    #endif

	if (index + arrPtr < endPtr)
		return arrPtr[index];
	else
		return arrPtr[index - _size];
}

 template<typename T>
 void CyclicArray<T>::operator << (size_t shift){
	#ifdef VALIDATION
	  if (shift > _size) shift %= _size;
    #endif

	arrPtr += shift; 
	if (arrPtr >= endPtr)
		arrPtr -= _size;
}

template<typename T>
void CyclicArray<T>::operator >> (size_t shift){
	#ifdef VALIDATION
		if (shift > _size) shift %= _size;
    #endif
		
	arrPtr -= shift;
	if (arrPtr < endPtr)
		arrPtr += _size;
}

template<typename T>
inline T& CyclicArray<T>::operator ++ (int){
	if (circleIdx == _size)
		circleIdx = 0;
	return (*this)[circleIdx++];
}

template<typename T>
inline T& CyclicArray<T>::operator ++ (){
	if (++circleIdx == _size)
		circleIdx = 0;
	return (*this)[circleIdx];
}

template<typename T>
inline T& CyclicArray<T>::operator -- (int){ //Постфиксный декремент
	if (circleIdx == 0)
		circleIdx = _size;
	return (*this)[circleIdx--];
}

template<typename T>
inline T& CyclicArray<T>::operator -- (){//префиксный декремент
	if (circleIdx == 0)
		circleIdx = _size;
	return (*this)[--circleIdx];
}

template<typename T>
void CyclicArray<T>::push_back(const T &newVal){
	(*this)[0] = newVal;
	(*this) << 1;
}

template<typename T>
void CyclicArray<T>::push_front(const T &newVal){
	 (*this) >> 1;
	 (*this)[0] = newVal;
}