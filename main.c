#include <stdio.h>
// подключаем 2 заголовочных файла для генерации
// случайных чисел
#include <stdlib.h>
#include <time.h>
// #include <string.h>

// для вычисления среднего значения количества
// перестановок и сравнений для 4-х сортировок массивов
int b_swaps = 0;
int b_comparisons = 0;
// т.к. quick_sort реализована рекурсивно, то ввожу ещё
// две переменные для вычисления среднего значения
int q_swaps = 0;
int q_comparisons = 0;
int q_swaps_sum = 0;
int q_comparisons_sum = 0;

void print_array(int size, int* array, char type)
{
    switch(type)
    {
        case 'a':
                 printf("Sorted (ascending) array:\n");
                 break;
        case 'd':
                printf("Back-sorted (descending) array:\n");
                break;
        case 'r':
                printf("Random array:\n");
                break;
        case 'b':
                printf("Array after bubble sort:\n");
                break;
        case 'q':
                printf("Array after quick sort:\n");
                break;
        default:
                printf("Some array.\n");
    }
    for (int i = 0; i <size; i++)
    {
        printf("%d ", array[i]);
    }
    printf("\n");
}

void print_swaps_comparisons_info(int swaps, int comparisons)
{
    printf("Number of swaps: %d\n", swaps);
    printf("Number of comparisons: %d\n\n", comparisons);
}

void create_random_el_array(int size, int* array)
{

    for (int i = 0; i < size; i++)
    {
      // НЕСКОЛЬКО ВАРИАНТОВ ГЕНЕРАЦИИ:
      // 1) array[i] = rand();
      // 2) rand() % (max + 1 - min) + min;
      // для генерации чисел из диапазона [min, max]
      // ! для удобства возьму диапазон [-100_000, 100_000]
      // array[i] = rand() % 101;
      // 3) лучший метод генерации:
      // min + rand() / (RAND_MAX / (max - min + 1) + 1);
       array[i] = -100000 + rand() / (RAND_MAX / 200002);
    }
}
// функции сравнения двух целых чисел для библиотечной qsort
int cmp_for_qsort_ascending(const void* a, const void* b)
{
    int dereferenced_a = *((const long long*) a);
    int dereferenced_b = *((const long long*) b);
    if (dereferenced_a - dereferenced_b > 0)
    {
        return 1;
    }
    if (dereferenced_a - dereferenced_b < 0)
    {
        return -1;
    }
    return 0;
}

int cmp_for_qsort_descending(const void* a, const void* b)
{
    int dereferenced_a = *((const long long*) a);
    int dereferenced_b = *((const long long*) b);
    if (dereferenced_a - dereferenced_b > 0)
    {
        return -1;
    }
    if (dereferenced_a - dereferenced_b < 0)
    {
        return 1;
    }
    return 0;
}
// неубывание (т.е. след больше или равен предыдущему)
void create_sorted_array(int size, int* array)
{
    create_random_el_array(size, array);
    qsort(array, size, sizeof(int), cmp_for_qsort_ascending);
}
// невозрастание (т.е. след меньше или равен предыдущему)
void create_back_sorted_array(int size, int* array)
{
    create_random_el_array(size, array);
    qsort(array, size, sizeof(int), cmp_for_qsort_descending);
}
// сравнений порядка O(n^2)
void bubble_sort_and_print(int size, int* array)
{
    // я хочу немного поменять классический bubble sort
    // и сделать алгоритм адаптивным, т.е. чтобы он менял своё
    // поведение в процессе работы
    // если я прошлась по массиву 1 раз и сделала 0 перестановок,
    // то это значит, что массив отсортирован и нет смысла проверять его дальше:
    int is_sorted_flag;
    int swaps = 0;
    int comparisons = 0;

    // всего шагов сортировки size - 1
    // поэтому внешний цикл имеет size - 1 итераций
    // индекс i также является счётчиком смещений, т.к. на
    // каждом этапе мы получаем +1 отсортированный элемент, то
    // количество сравнений уменьшается на 1
    // поэтому из j вычитаем i
    for (int i = 0; i < size - 1; i++)
    {
        is_sorted_flag = 1;
        for (int j = 0; j < size - 1 - i; j++)
        {
            comparisons++;
            // т.к. в моём варианте сортировка по неубыванию, то если предыдущий
            // элемент больше следующего, то меняем местами
            // при этом если значения одинаковые, то нет смысла делать лишнюю перестановку
            if (array[j] > array[j + 1])
            {
                swaps++;
                int temp = array[j];
                array[j] = array[j + 1];
                array[j + 1] = temp;
                // была сделана по крайней мере 1 перестановка
                // поэтому нужно продолжить сортировку
                is_sorted_flag = 0;
            }
        }
        // останавливаю сортировку, если было 0 перестановок на данной
        // итерации внешнего цикла
        if (is_sorted_flag == 1)
        {
            break;
        }
    }
    // обновляем глобальные переменные
    b_swaps += swaps;
    b_comparisons += comparisons;

    print_array(size, array, 'b');
    print_swaps_comparisons_info(swaps, comparisons);
}
// вспомогательная функция для quick_sort, сортирующая один элемент
int sort_one_element(int lower, int higher, int size, int* array)
{
    int swaps = 0;
    // инициализирую как 2, т.к. дальше в 2-х while будет 1 'терминальное'
    // сравнение (т.е. после него внутрь цикла управление не перейдёт),
    // а обновляю счётчик я именно внутри цикла
    int comparisons = 2;
    // pivot - элемент, который будем сортировать
    int pivot = array[lower];
    // нижняя и верхняя грань сортировки(индексы)
    // нужны для дальнейшей рекурсии в функции
    // quick_sort
    int i = lower;
    int j = higher - 1;
    // для дальнейших перестановок
    int temp = 0;
    while (i < j)
    {
        while ((array[i] <= pivot) && (i < size - 1))
        {
            comparisons++;
            i++;
        }
        while ((array[j] > pivot) && (j >= 0))
        {
            comparisons++;
            j--;
        }
        if (i < j)
        {
            swaps++;
            temp = array[j];
            array[j] = array[i];
            array[i] = temp;
        }
    }
    // мы нашли позицию, в которой наш pivot будет
    // отсортирован, т.е. все элементы меньшим,
    // чем pivot будут слева, а больше - справа от pivot
    // позиция задаётся индексом j, поэтому меняем соответствующие
    // элементы местами:
    swaps++;
    temp = array[lower];
    array[lower] = array[j];
    array[j] = temp;
    // обновляю глобальныe счётчики
    q_swaps += swaps;
    q_comparisons += comparisons;
    return j;
}
// рекурсивная реализация quick_sort
void quick_sort_recursive(int size, int* array, int lower, int higher)
{
    int j = 0;
    if (lower < higher)
    {
        // сортируем элемент, в качестве границ берём
        // границы исходного массива
        j = sort_one_element(lower, higher, size, array);
        // а теперь рекурсивно сортируем левый и правый подмассивы
        quick_sort_recursive(size, array, lower, j);
        quick_sort_recursive(size, array, j + 1, higher);
    }
}
// "обертка" для функции quick_sort_recursive,
// чтобы прототип соответсвовал условию задания +
// для удобного вывода результатов
void quick_sort_and_print(int size, int* array)
{
    quick_sort_recursive(size, array, 0, size);
    print_array(size, array, 'q');
    print_swaps_comparisons_info(q_swaps, q_comparisons);
    // обновляю глобальные счётчики для вычисления среднего значения
    // а также зануляю счётчики для подсчёта для следующего массива
    q_swaps_sum += q_swaps;
    q_swaps = 0;
    q_comparisons_sum += q_comparisons;
    q_comparisons = 0;
}

int main(void)
{
    int size;
    char get_size_input = 'y';
    do
    {
        printf("Type initial array size (from 1 to 100 000): ");
        scanf("%d", &size);
        if (size < 1 || size > 100000)
        {
            printf("You've typed unacceptable array size : %d", size);
            printf("\nDo you want to try again? (y/n)\n");
            // без пробела scanf считает \n из предыдущей команды printf
            // и запишет его в get_size_input
            scanf(" %c", &get_size_input);
        }
        else
        {
            get_size_input = 'n';
        }

    }while (get_size_input != 'n');

    int* array1 = malloc(size * sizeof(int));
    int* array2 = malloc(size * sizeof(int));
    int* array3 = malloc(size * sizeof(int));
    int* array4 = malloc(size * sizeof(int));
    // Тест на корректность функций генерации массива:
    // srand долен быть здесь!!!

    // ТЕСТЫ!!!


    // Bubble sort
    srand(time(NULL));
    create_random_el_array(size, array3);
    print_array(size, array3, 'r');
    bubble_sort_and_print(size, array3);


    create_random_el_array(size, array4);
    print_array(size, array4, 'r');
    bubble_sort_and_print(size, array4);


    create_sorted_array(size, array2);
    print_array(size, array2, 'a');
    // bubble_sort - устойчивый алгоритм, т.е.
    // если массив уже отсортирован, то он останется отсортированным,
    // при этом он закончит работу после первого шага сортировки
    // т.е.количество сравнений - array_size - 1
    // количество перестановок - 0
    bubble_sort_and_print(size, array2);


    create_back_sorted_array(size, array1);
    print_array(size, array1, 'd');
    // а вот тут уже придётся поменять местами все элементы,
    // из курса линейной алгебры помню, что для того,
    // чтобы из натуральной перестановки сделать обратную
    // нужно n(n-1)/2 транспозиций
    // применительно к сортировке массива
    // количество сравнений будет n(n-1)/2
    // количество перестановок <= n(n-1)/2
    // т.к. могут быть одинаковые элементы, а их я не меняю
    bubble_sort_and_print(size, array1);

    //округление вниз
    printf("Average number of swaps for 4 sorted arrays: %d\n", b_swaps/4);
    printf("Average number of comparisons for 4 sorted arrays: %d\n", b_comparisons/4);


    // Quick sort

    create_random_el_array(size, array3);
    print_array(size, array3, 'r');
    quick_sort_and_print(size, array3);

    create_random_el_array(size, array4);
    print_array(size, array4, 'r');
    quick_sort_and_print(size, array4);


    create_sorted_array(size, array2);
    print_array(size, array2, 'a');
    quick_sort_and_print(size, array2);


    create_back_sorted_array(size, array1);
    print_array(size, array1, 'd');
    quick_sort_and_print(size, array1);
/*
    int* array5 = malloc(sizeof(int) * 9);
    memcpy(array5, (int[]){10, 16, 8, 12, 15, 6, 3, 9, 5}, 9 * sizeof(int));
    quick_sort_and_print(9, array5);
*/
    //округление вниз
    printf("Average number of swaps for 4 sorted arrays: %d\n", q_swaps_sum / 4);
    printf("Average number of comparisons for 4 sorted arrays: %d\n", q_comparisons_sum / 4);


    free(array1);
    free(array2);
    free(array3);
    free(array4);
  //  free(array5);

    return 0;
}
