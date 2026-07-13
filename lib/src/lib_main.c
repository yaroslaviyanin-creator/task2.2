/*
lib_main.c - главный модуль библиотеки.
Янин Ярослав Иванович
Группа МК-101
*/

#include <stdio.h>
#include <stdlib.h>
#include "config.h"

// Функция для вывода в шестнадцатеричном виде
// <chunk> - Указатель на буфер
// <bytes_available> - кол-во байтов которые мы считали из файла
// <group_size> - размер буфера
void print_hex_group(const unsigned char* chunk, int bytes_available, int group_size) {
    const char hex_chars[] = "0123456789ABCDEF";
    // Little-Endian:
    for (int i = group_size - 1; i >= 0; i--) {
        unsigned char byte = 0; // По умолчанию байт равен 00
        // Если индекс попадает в реально прочитанные данные, берем настоящий байт
        if (i < bytes_available) {
            byte = chunk[i];
        }
        // Извлекаем левый и правый полубайты
        char high_nibble = hex_chars[(byte >> 4) & 0x0F];
        char low_nibble = hex_chars[byte & 0x0F];
        // Выводим
        printf("%c%c", high_nibble, low_nibble);
    }
}


// Функция для вывод на экран
// <buffer> - Указатель на массив считанных байтов
// <bytes_read> - Количество реально считанных байтов, находящихся в буфере
// <current_offset> - Текущее смещение от начала файла
// <cfg> - Указатель на структуру с аргументамми
void print_standard_format(const unsigned char* buffer, int bytes_read, int current_offset, Config* cfg) {
    // Выводим текущее смещение
    printf("%08x: ", current_offset);

    // Выводим шестнадцатеричные группы
    for (int i = 0; i < cfg->count; i++) {
        int group_start = i * cfg->group_size;

        if (group_start >= bytes_read) {
            // Если данные закончились, но строка не полная, печатаем пробелы для выравнивания
            // Каждый кусок занимает (group_size * 2) символов + 1 пробел
            for (int j = 0; j < cfg->group_size * 2 + 1; j++) {
                printf(" ");
            }
            continue;
        }

        // Вычисляем, сколько байт доступно для текущей группы
        int chunk_bytes = bytes_read - group_start;
        if (chunk_bytes > cfg->group_size) {
            chunk_bytes = cfg->group_size;
        }

        print_hex_group(&buffer[group_start], chunk_bytes, cfg->group_size);
        printf(" "); // Пробел между группами
    }

    // Если group_size == 1, то выводим символьное представление
    if (cfg->group_size == 1) {
        printf(" |");
        for (int i = 0; i < bytes_read; i++) {
            unsigned char c = buffer[i];
            // Печатаем только видимые символы ASCII (от 32 до 126)
            if (c >= 32 && c <= 126) {
                printf("%c", c);
            }
            else {
                printf("."); // Непечатные символы заменяем точкой
            }
        }
        printf("|");
    }
    printf("\n");
}


void process_file(const char* filepath, Config* cfg) {
    // Открываем файл
    FILE* file = fopen(filepath, "rb");
    if (file == NULL) {
        fprintf(stderr, "Error: Cannot open file '%s'\n", filepath);
        return;
    }

    // Проверяем, подходит ли offset под файл + сдвигаем курсор на offset байт
    fseek(file, 0, SEEK_END);          // Сдвигаемся в конец файла
    long file_size = ftell(file);      // Узнаём размер файла

    if (cfg->offset > file_size) {
        fprintf(stderr, "Warning: offset exceeds file size.\n");
        fclose(file);
        return;
    }
    // Сдвигаем указатель на offset байт
    if (cfg->offset > 0) fseek(file, cfg->offset, SEEK_SET);


    // Вычисляем размер буфера для одной строки вывода и выделяем память
    int line_size = cfg->group_size * cfg->count;               // line_size - кол-во байт нужное для считывания за блок
    unsigned char* buffer = (unsigned char*)malloc(line_size);
    if (buffer == NULL) {
        fprintf(stderr, "Error: Memory allocation failed\n");
        fclose(file);
        return;
    }


    int current_offset = cfg->offset;   // Наше положение в файле
    int total_read = 0;                 // Сколько байт мы уже прочитали суммарно


    // Главный цикл чтения файла блоками с учетом лимита (-l) и конца файла
    while (1) {
        // Проверяем ограничение по количеству выводимых из файла байт (-l)
        if (cfg->length != -1 && total_read >= cfg->length) {
            break;
        }

        // Вычисляем, сколько байт нам нужно прочитать на этом шаге
        int bytes_to_read = line_size;                                      // bytes_to_read - кол-во байт которое считаем из файла на этом шаге
        if (cfg->length != -1 && (cfg->length - total_read) < line_size) {
            // Если до лимита осталось меньше, чем размер полной строки
            bytes_to_read = cfg->length - total_read;
        }

        // Читаем данные в буфер
        size_t bytes_read = fread(buffer, 1, bytes_to_read, file);
        if (bytes_read == 0) {
            break; // Конец файла или ошибка чтения
        }


        // Если форматная строка не задана, используем стандартный вывод
        if (cfg->format_str == NULL) {
            print_standard_format(buffer, bytes_read, current_offset, cfg);
        }
        else {
            // Вывод если задана форматная строка
        }


        // Обновляем счетчики
        total_read += bytes_read;
        current_offset += bytes_read;
    }

    // Очищаем память и закрываем файл
    free(buffer);
    fclose(file);
}
