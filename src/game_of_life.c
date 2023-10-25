#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define MAXX 80
#define MAXY 25
#define DEF_DELAY 300
#define MAX_DELAY 3000

#define CL_ALIVE '@'
#define CL_DEAD '.'

// ====================================

// инициализация поля из stdin или генератором
// возвращает 1 при успехе и 0 при ошибке
int init_field(char field[MAXY][MAXX]);

// пытается читать поле из stdin
// если прочиталось — возвращает 1
// если нет данных - возвращает 0
// если в данных ошибка - возвращает -1
int read_field(char field[MAXY][MAXX]);

// пытается читать ряд
// возвращает количество прочитанных символов при успехе
// или -1 при ошибке
int read_row(char row[MAXX]);

// генерация поля
void generate_field(char field[MAXY][MAXX]);

// ====================================

// читает задержку между поколениями или возвращает по умолчанию
unsigned int read_delay();

// ====================================

// выводит приветствие
void print_greeting();

// предлагает ввести задержку
void print_input_delay();

// выводит ошибку n/a
void print_error();

// вывод поля
void print_field(char field[MAXY][MAXX]);

// ====================================

// подсчёт соседей у клетки в координатах
int count_neighbours(char field[MAXY][MAXX], int row, int col);

// нормализация координаты для зацикливания поля
int normalize(int n, unsigned int count);

// ====================================

// вычисляет, что сделать для клетки по количеству соседей
// -1 - убить клетку
// 0 - не трогаем клетку
// 1 - оживить клетку
int mutation(int alive, int neigbours);

// обновление поля, возвращаем 1, если поколение
// сгенерировалось и есть живые клетки, иначе - 0
int update_field(char field[MAXY][MAXX]);

// ====================================

int main() {
    // приветствие + инструкция по вводу
    print_greeting();

    // наше поле с клетками
    char field[MAXY][MAXX];

    // пытаемся инициализировать поле
    if (init_field(field) == 0) {
        // при ошибке, выводим n/a и выходим
        print_error();
        return 1;
    }

    // пытаемся прочитать задержку
    print_input_delay();
    unsigned int delay = read_delay();
    if (delay == 0 || delay > MAX_DELAY) {
        // при нулевой задержке или задержке
        // больше максимума вернуть ошибку
        print_error();
        return 2;
    }

    do {
        // выводим текущее поколение на экран
        print_field(field);
        // задержка перед следующим поколением
        usleep(delay * 1000);
    } while (update_field(field));

    // выводим поле в последний раз
    // (после последнего обновления)
    print_field(field);

    return 0;
}

// ====================================

// инициализация поля из stdin или генератором
int init_field(char field[MAXY][MAXX]) {
    // пытаемся читать ввод с полем,
    // смотрим, как получилось
    switch (read_field(field)) {
    case 0:
        // данных нет, генерируем поле
        generate_field(field);
        // возвращаем 1
        return 1;
    case 1:
        // поле прочиталось, возвращаем 1
        return 1;
    default:
        // ошибка в данных, возвращаем 0
        return 0;
    }
}

int read_field(char field[MAXY][MAXX]) {
    for (int row = 0; row < MAXY; row++) {
        int row_count = read_row(field[row]);
        if (row_count == 0) {
            // данных не было, возвращаем 0
            return 0;
        } else if (row_count == MAXX) {
            // ряд успешно прочитан
            continue;
        } else {
            // ошибка чтения, возвращаем -1
            return -1;
        }
    }

    return 1;
}

int read_row(char row[MAXX]) {
    int count = 0;
    while (count <= MAXX) {
        char character;
        if (scanf("%c", &character) != 1 || character == '\n') {
            // при отсуствии ввода или переносе строки прерываем чтение
            break;
        }

        if (character != CL_ALIVE && character != CL_DEAD) {
            // не известный символ, выходим с ошибкой
            return -1;
        }

        // выставляем 1 при прочтении активной клетки
        *(row + count) = character == CL_ALIVE;
        // смещаем ячейку
        count++;
    }
    return count;
}

void generate_field(char field[MAXY][MAXX]) {
    srand(time(NULL));

    int count;
    do {
        count = 0;
        // пытаемся сгенерировать поле
        for (int row = 0; row < MAXY; row++) {
            for (int col = 0; col < MAXX; col++) {
                // небольшая вероятность живых клеток
                // int alive = rand() < RAND_MAX / 20;
                int alive = rand() % 2;
                field[row][col] = alive;
                // считаем живые клетки
                count += alive;
            }
        }
        // повторяем, пока не выйдет хотя бы 3.3% живых клеток
    } while (count < (MAXX * MAXY) / 20);
}

// ====================================

unsigned int read_delay() {
    unsigned int delay;
    if (scanf("%u", &delay) != 1) {
        return DEF_DELAY;
    }

    return delay;
}

// ====================================

void print_greeting() {
    printf("Нажмите ввод для генерированного поля или введите поле вручную.\n");
    printf("Игровое поле имеет размер %d x %d символов, на котором живая\n", MAXX, MAXY);
    printf("клетка - символ '%c', а мёртвая клетка - символ '%c'.\n", CL_ALIVE, CL_DEAD);
    printf("[!] Важно: любой символ кроме '%c' и '%c' считается ошибкой данных!\n", CL_ALIVE, CL_DEAD);
}

void print_input_delay() {
    printf("Введите задержку между поколенями в мс или нажмите ввод для значения по умолчанию (%d мс): ", DEF_DELAY);
}

void print_error() {
    printf("n/a\n");
}

void print_field(char field[MAXY][MAXX]) {
    // чистим экран
    system("clear");

    for (int row = 0; row < MAXY; row++) {
        for (int col = 0; col < MAXX; col++) {
            if (field[row][col]) {
                putchar(CL_ALIVE);
            } else {
                putchar(CL_DEAD);
            }
        }
        putchar('\n');
    }
}

// ====================================

int count_neighbours(char field[MAXY][MAXX], int row, int col) {
    // счётчик количества соседей
    int count = 0;

    // проходимся по 8 клеткам вокруг
    // используя относительные координаты [-1, 0, 1]
    // для обоих осей
    for (int r = -1; r <= 1; r++) {
        for (int c = -1; c <= 1; c++) {
            // если мы в нашей клетке ([col, row]),
            // то пропускаем (себя не считаем)
            if (r == 0 && c == 0) {
                continue;
            }

            // нормализуем координаты, суммируя относительные
            // координаты с заданными, чтобы зациклить поле
            int r_norm = normalize(r + row, MAXY);
            int c_norm = normalize(c + col, MAXX);

            // проверяем, есть ли по координатам клетка
            if (field[r_norm][c_norm]) {
                // если клетка есть, увеличиваем счётчик соседей
                count++;
            }
        }
    }

    return count;
}

int normalize(int n, unsigned int count) {
    if (n < 0) {
        return n % -count;
    } else {
        return n % count;
    }
}

// ====================================

int mutation(int alive, int neigbours) {
    if (alive == 0) {
        // в пустой (мёртвой) клетке
        if (neigbours == 3) {
            // соседствуют три живые клетки, зарождается жизнь
            return 1;
        }
    } else {
        // у живой клетки
        if (neigbours == 2 || neigbours == 3) {
            // если есть две или три живые соседки, то эта клетка продолжает
            // жить (оставляем состояние)
        } else {
            // в противном случае (если живых соседей меньше двух
            // или больше трёх) клетка умирает («от одиночества»
            // или «от перенаселённости»).
            return -1;
        }
    }

    // ничего не делаем
    return 0;
}

int update_field(char field[MAXY][MAXX]) {
    char new_field[MAXY][MAXX];

    // счётчики живых, оживлённых и убитых клеток
    int total = 0, added = 0, removed = 0;

    // считаем количество живых соседей для каждой ячейки поля
    for (int row = 0; row < MAXY; row++) {
        for (int col = 0; col < MAXX; col++) {
            new_field[row][col] = field[row][col];

            // количество живых соседей в клетке
            int neigbours = count_neighbours(field, row, col);
            // жива ли сама клетка (1 - да, 0 - нет)
            int alive = field[row][col];
            // смотрим, как меняется клетка поля
            int action = mutation(alive, neigbours);
            if (action > 0) {
                // оживляем клетку
                new_field[row][col] = 1;
                // считаем живую клетку
                total++;
                // считаем оживлённую клетку
                added++;
            } else if (action < 0) {
                // убиваем клетку
                new_field[row][col] = 0;
                // считаем убитую клетку
                removed++;
            } else if (alive) {
                // если клетка была жива, считаем её
                total++;
            }
        }
    }

    for (int row = 0; row < MAXY; row++) {
        for (int col = 0; col < MAXX; col++) {
            field[row][col] = new_field[row][col];
        }
    }

    // поколение успешно, если есть живые,
    // и были оживлены или убиты клетки
    return total > 0 && (added || removed);
}
