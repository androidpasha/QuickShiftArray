Размер cyclicArray 20, stdArr 400, vector 12
Размер масивов по 100 ячеек;
Количество повторений при замере времени 100000 раз.


Время сдвига:

cyclicArray:            11 мс
classicArr:             511 мс
std::array:             511 мс
std::vector:            517 мс

Заполнение forLoop:

cyclicArray:            3630 мс
std::array:             312 мс
std::vector:            308 мс
classicArr:             313 мс

Заполнение range-based for loop:

cyclicArray:            1398 мс
std::array:             313 мс
std::vector:            311 мс
classicArr:             312 мс

Заполнение for loop через итераторы:

cyclicArray:            1272 мс
std::array:             312 мс
std::vector:            306 мс

Круговое циклическое заполнение массива:

cyclicArray             53 мс
std::array:             1632 мс
std::vector:            882 мс
classicArr:             755 мс

Вставка в конец со сдвигом остальных элементов влево:

cyclicArray:            51 мс
classicArr:             516 мс
std::array:             516 мс
std::vector:            519 мс