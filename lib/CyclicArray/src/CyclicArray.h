/* Библиотека сдвигового массива.
Основное преимущество над классическим массивом скорость сдвига O(1) и составляет
примерно 0,1 мкс для ESP8266. Для сравнения сдвиг класического массива из
занимает примерно 4,5 мкс.
(например был массив 0123 при сдвиге влево на 1 вышло 1230)
Присутствует функция push_back и push_front для добавления новых данных в края массива,
при этом массив сдвигается влево(вправо) на один элемент. противоположный вставке
элемент исчезает.
---
При создании экземпляра на куче выделяется память под новый массив.
Размер массива можно изменять мтодом setSize. Данные сохраняються в пределах нового размера.
Библиотека хранит номер элемента, с которого при вызове оператора [index]
начинается отсчет "нулевого элемента для пользователя". При сдвиге массива
изменяется номер элемента начала отсчета вместо перезаписи всего массива.
---
Класс CyclicArray шаблонный и может создавать массивы с любыми типами данных:
CyclicArray<ТИП_ДАННЫХ> НАЗВАНИЕ_МАССИВА(КОЛИЧЕСТВО_ЯЧЕЕК);
Например CyclicArray<CyclicArray<int, 4>, 4> arr; создаст двумерный массив 4х4.
Обращение к ячейкам как в обычном массиве: НАЗВАНИЕ_МАССИВА[ИНДЕКС]; НАЗВАНИЕ_МАССИВА[ИНДЕКС][ИНДЕКС] для 2D
Добавление новых данных в конец со сдвигом массива влево:
НАЗВАНИЕ_МАССИВА.push_back(НОВОЕ_ЗНАЧЕНИЕ);
Добавление новых данных в начало со сдвигом массива вправо:
НАЗВАНИЕ_МАССИВА.push_front(НОВОЕ_ЗНАЧЕНИЕ);
---
Сдвиг массива: НАЗВАНИЕ_МАССИВА << КОЛИЧЕСТВО_ШАГОВ_СДВИГА;
НАЗВАНИЕ_МАССИВА >> КОЛИЧЕСТВО_ШАГОВ_СДВИГА;
Количество элементов в массиве size_t: НАЗВАНИЕ_МАССИВА.size();
Пройтись по всем элементам: for(size_t i = 0; i < НАЗВАНИЕ_МАССИВА.size(); i++){}
---
Недостатки:
Скорость доступа к элементам дольше чем у классического массива из-за подсчета индексов.
*/

#pragma once
#include <iterator>
#include <initializer_list>
#ifdef ARDUINO
#include<Arduino.h>
#endif

template<typename T, size_t Size = 0>
class CyclicArray
{
	size_t _size{ 0 };			//хранит количество элементов массива
	T* beginPtr{ nullptr };		//указатель на начало созданного массив на куче
	T* offsetPtr{ nullptr };	//указатель начального положение массива с учетом сдвига массива
	T* endPtr{ nullptr };		//указатель на конец массива
	size_t circleIdx{ 0 };		//индекс для обращения к массиву через операторы ++ и --
public:
	class Iterator;
	CyclicArray();									//конструктор по умолчанию
	CyclicArray(size_t size);						//конструктор. создает на куче массив
	CyclicArray(std::initializer_list<T> initList);	//конструктор инициализации списком
	CyclicArray(const CyclicArray& other);			//конструктор копирования для передачи по значению
	CyclicArray(CyclicArray&& other) noexcept;
	CyclicArray& operator=(CyclicArray&& other);// перегрузка присваивания по перемещению
	CyclicArray& operator=(const CyclicArray& other);// перегрузка оператора присваивания
	CyclicArray(const char* str);					//инициализация с строкой
	//T* operator->() { return (*this); }
	T& operator [] (size_t index);					//возвращает указатель на элементу массива с номером index
	const T& operator [] (size_t index) const;					//возвращает указатель на элементу массива с номером index	
	void operator >> (size_t shift);				//сдвигает массив вправо (меняется индекс отсчета)
	void operator << (size_t shift);				//сдвигает массив влево
	T& operator ++ (int);							//постфиксный инкремент. Возвращает элемент с номером circleIdx++.
	T& operator ++ ();								//префиксный инкремент. Возвращает элемент с номером ++circleIdx.
	T& operator -- (int); 							//постфиксный декремент. Возвращает элемент с номером circleIdx--.
	T& operator -- ();								//префиксный декремент. Возвращает элемент с номером --circleIdx.
	size_t getCircleIdx()const { return circleIdx; } 		//возвращает положение кругового индекса для чтения/записи операторами ++ и --
	T& setCircleIdx(size_t idx) { circleIdx = idx; return (*this)[circleIdx]; }//устанавливает круговой индекс для чтения/записи операторами ++ и -- а также возвращает значение установленной позиции
	void setOffset(size_t offset) { offsetPtr = beginPtr + offset; }	//Сдвигает массив влево начиная с нулевого элемента без учета прежних сдвигов
	size_t getOffset() const { return offsetPtr - beginPtr; }			//Возвращает сдвиг
	void push_back(const T&);				//сдвигает массив влево на одну позицию и добавляет в последнюю элемент новые данные
	void push_front(const T&);				//сдвигает массив вправо на одну позицию и добавляет в начальную элемент новые данные
	size_t size() const { return _size; };						//возвращает количество элементов массива
	void setSize(size_t);
	Iterator begin() { return Iterator(beginPtr, offsetPtr, endPtr); } //Возвращает iterator. Необходим для работы цикла по диапазону 
	Iterator end() { return Iterator(beginPtr, offsetPtr - 1, endPtr); }
	~CyclicArray() { delete[] beginPtr; _size = 0; beginPtr = offsetPtr = endPtr = nullptr; };
};

template<typename T, size_t Size>
CyclicArray<T, Size>::CyclicArray() : _size(Size) {
	if (_size){
		beginPtr = new T[_size];
		offsetPtr = beginPtr;
		endPtr = beginPtr + _size;
	}
}

template<typename T, size_t Size>
CyclicArray<T, Size>::CyclicArray(size_t size) : _size(size) {
	beginPtr = new T[_size];
	offsetPtr = beginPtr;
	endPtr = beginPtr + _size;
}

template<typename T, size_t Size>
CyclicArray<T, Size>::CyclicArray(std::initializer_list<T> initList) {
	_size = initList.size();
	beginPtr = new T[_size];
	offsetPtr = beginPtr;
	endPtr = beginPtr + _size;
	std::copy(initList.begin(), initList.end(), beginPtr);
}

template<typename T, size_t Size>
CyclicArray<T, Size>::CyclicArray(const CyclicArray& other)
	:_size(other._size), circleIdx(other.circleIdx) {
	beginPtr = new T[_size];
	std::copy(other.beginPtr, other.endPtr, beginPtr);
	offsetPtr = beginPtr + (other.offsetPtr - other.beginPtr);
	endPtr = beginPtr + _size;
}
//конструктор переноса используется при создании новых объектов,
//оператор присваивания по перемещению - при присваивании уже существующих объектов.
template<typename T, size_t Size>
CyclicArray<T, Size>::CyclicArray(CyclicArray&& other) noexcept { // Конструктор переноса
	 if (this != &other) {
        //delete[] beginPtr; // т.к. конструктор переноса вызывается для новых объектов то и удалять нечего
        _size = std::exchange(other._size, 0); // Перемещение _size и сброс в 0
        circleIdx = std::exchange(other.circleIdx, 0); // Перемещение circleIdx и сброс в 0
        beginPtr = std::exchange(other.beginPtr, nullptr); // Перемещение beginPtr и сброс в nullptr
        offsetPtr = std::exchange(other.offsetPtr, nullptr);
        endPtr = std::exchange(other.endPtr, nullptr);
    }
}

template<typename T, size_t Size>
CyclicArray<T, Size>& CyclicArray<T, Size>::operator=(CyclicArray&& other) {
	if (this != &other) {
		delete[] beginPtr;
        _size = std::exchange(other._size, 0); // Перемещение _size и сброс в 0
        circleIdx = std::exchange(other.circleIdx, 0); // Перемещение circleIdx и сброс в 0
        beginPtr = std::exchange(other.beginPtr, nullptr); // Перемещение beginPtr и сброс в nullptr
        offsetPtr = std::exchange(other.offsetPtr, nullptr);
        endPtr = std::exchange(other.endPtr, nullptr);
	}
	return *this;
}

template<typename T, size_t Size>
CyclicArray<T, Size>& CyclicArray<T, Size>::operator=(const CyclicArray& other) {
	if (this != &other) {
		delete[] beginPtr;
		_size = other._size;
		circleIdx = other.circleIdx;
		beginPtr = new T[_size];
		std::copy(other.beginPtr, other.endPtr, beginPtr);
		offsetPtr = beginPtr + (other.offsetPtr - other.beginPtr);
		endPtr = beginPtr + _size;
	}
	return *this;
}

template<typename T, size_t Size>
T& CyclicArray<T, Size>::operator[](size_t index) {
	if (index + offsetPtr < endPtr)
		return offsetPtr[index];
	else
		return offsetPtr[index - _size];
}

template<typename T, size_t Size>
const T& CyclicArray<T, Size>::operator[](size_t index) const {
	return const_cast<CyclicArray*>(this)->operator[](index);
}

template<typename T, size_t Size>
void CyclicArray<T, Size>::operator << (size_t shift) {
	offsetPtr += shift;
	if (offsetPtr >= endPtr)
		offsetPtr -= _size;
}

template<typename T, size_t Size>
void CyclicArray<T, Size>::operator >> (size_t shift) {
	offsetPtr -= shift;
	if (offsetPtr < endPtr)
		offsetPtr += _size;
}

template<typename T, size_t Size>
T& CyclicArray<T, Size>::operator ++ (int) {
	if (circleIdx == _size)
		circleIdx = 0;
	return (*this)[circleIdx++];
}

template<typename T, size_t Size>
T& CyclicArray<T, Size>::operator ++ () {
	if (++circleIdx == _size)
		circleIdx = 0;
	return (*this)[circleIdx];
}

template<typename T, size_t Size>
T& CyclicArray<T, Size>::operator -- (int) { //Постфиксный декремент
	if (circleIdx == 0)
		circleIdx = _size;
	return (*this)[circleIdx--];
}

template<typename T, size_t Size>
T& CyclicArray<T, Size>::operator -- () {//префиксный декремент
	if (circleIdx == 0)
		circleIdx = _size;
	return (*this)[--circleIdx];
}

template<typename T, size_t Size>
void CyclicArray<T, Size>::push_back(const T& newVal) {
	(*this)[0] = newVal;
	(*this) << 1;
}

template<typename T, size_t Size>
void CyclicArray<T, Size>::push_front(const T& newVal) {
	(*this) >> 1;
	(*this)[0] = newVal;
}

template<typename T, size_t Size>
CyclicArray<T, Size>::CyclicArray(const char* str) {
	_size = strlen(str) + 1;
	beginPtr = new char[_size];
	offsetPtr = beginPtr;
	endPtr = beginPtr + _size;
	strcpy(beginPtr, str);
}

template<typename T, size_t Size>
void CyclicArray<T, Size>::setSize(size_t newSize) {
		T* tmp = new T[newSize];
		if (newSize < _size)	
        	std::copy(beginPtr, beginPtr + newSize, tmp);
		else
			std::copy(beginPtr, endPtr, tmp);
		delete[] beginPtr;
		beginPtr = tmp;
		offsetPtr = beginPtr;
		endPtr = beginPtr + newSize;
		_size = newSize;
		circleIdx = 0;
}

template<typename T, size_t Size>
class CyclicArray<T, Size>::Iterator : public std::iterator<std::random_access_iterator_tag, T> {
	size_t size; T* beginPtr, * endPtr, * offsetPtr, * offsetEndPtr; 	//размер массива, смещение, указатели на начало, конец массива и нулевую позицию, заданную пользователем в диапазоне массива
public:
	Iterator() {} //конструктор по умолчанию
	Iterator(const Iterator& other) = default;//конструктор копирования
	Iterator (Iterator&& other) = default; //конструктор переноса
	explicit Iterator(T* beginPtr, T* offsetPtr, T* endPtr) :
		size(endPtr - beginPtr), beginPtr(beginPtr), endPtr(endPtr), offsetPtr(offsetPtr), offsetEndPtr(offsetPtr + size + 1) {}
	T* operator ->	() { return offsetPtr; }
	Iterator& operator ++	() { ++offsetPtr; return *this; }
	Iterator& operator --	() { --offsetPtr; return *this; }
	Iterator	operator +	(int n) const { return Iterator(beginPtr, offsetPtr + n, endPtr); }
	Iterator	operator -	(int n) const { return Iterator(beginPtr, offsetPtr - n + 1, endPtr); }
	int			operator -	(const Iterator& other) const { return offsetPtr - other.offsetPtr; }
	Iterator& operator +=	(size_t n) { offsetPtr += n; return *this; }
	Iterator& operator -=	(size_t n) { offsetPtr -= n; return *this; }
	bool		operator !=	(const Iterator& other) const { return (offsetPtr != other.offsetEndPtr) and (offsetPtr + size + 1 != other.offsetEndPtr); } // and (offsetPtr != other.beginPtr - 1) and (other.offsetPtr != beginPtr - 1);}
	bool		operator ==	(const Iterator& other) const { return !(*this != other); }
	bool		operator <	(const Iterator& other) const { return offsetPtr < other.offsetPtr; }
	bool		operator >	(const Iterator& other) const { return offsetPtr > other.offsetPtr; }
	bool		operator <=	(const Iterator& other) const { return offsetPtr <= other.offsetPtr; }
	bool		operator >=	(const Iterator& other) const { return offsetPtr >= other.offsetPtr; }
	T& operator [] (size_t index) { return *(Iterator(beginPtr, offsetPtr + index, endPtr)); }
	T& operator *() const;
	Iterator& operator =	(const Iterator& other);
	explicit operator void* () const;
};

template<typename T, size_t Size>
T& CyclicArray<T, Size>::Iterator::operator*() const {
	if (offsetPtr >= beginPtr and offsetPtr < endPtr)
		return *offsetPtr;

	if (offsetPtr >= endPtr + size) { 
		size_t dif = offsetPtr - endPtr;
		return *(offsetPtr - size * (dif / size + 1)); }

	if (offsetPtr >= endPtr) 
		return *(offsetPtr - size);

	return *(offsetPtr + size);
}

template<typename T, size_t Size>
typename CyclicArray<T, Size>::Iterator& CyclicArray<T, Size>::Iterator::operator=(const Iterator& other) {
	if (this != &other) {
		size = other.size;
		beginPtr = other.beginPtr;
		endPtr = other.endPtr;
		offsetPtr = other.offsetPtr;
		offsetEndPtr = other.offsetEndPtr;
	}
	return *this;
}

template<typename T, size_t Size>
CyclicArray<T, Size>::Iterator::operator void* () const {
		if (offsetPtr >= beginPtr and offsetPtr < endPtr) return static_cast<void*>(offsetPtr);
		if (offsetPtr >= endPtr) return static_cast<void*>(offsetPtr - size);
		return static_cast<void*>(offsetPtr + size);
}