/*
main.c - главный модуль программы.
Янин Ярослав Иванович
Группа МК-101
*/

#include <stdio.h>
#include <stdlib.h>
#include "getopt.h"

// Структура для хранения аргументов
typedef struct {
    char* file_name;    // -i имя выводимого файла
    int offset;         // -o смещение от начала файла, с которого выводится содержимое, если не задано, то выводить с начала файла (по умолчанию 0)
    int length;         // -l количество выводимых из файла байт, если не задано, то выводить всё (по умолчанию -1)
    int group_size;     // -g размер кусочка (по умолчанию 1)
    int count;          // -n количество кусочков, выводимых в одной строке (по умолчанию 16)
    char* dir;          // -d директория, из которой выводить файлы
    char* format_str;   // -f указание форматной строки, определяющей формат вывода каждой строки
} Config;

int main(int argc, char* argv[]) {
    // Инициализируем структуру значениями по умолчанию
    Config cfg = {
        .file_name = NULL,
        .offset = 0,
        .length = -1,
        .group_size = 1,
        .count = 16,
        .dir = NULL,
        .format_str = NULL
    };

    int opt;

    while ((opt = getopt(argc, argv, "i:o:l:g:n:d:f:")) != -1) {
        switch (opt) {
        case 'i':
            cfg.file_name = optarg;
            break;
        case 'o':
            if (sscanf(optarg, "%d", &cfg.offset) != 1) {
                fprintf(stderr, "Error: Option -o requires an integer argument.\n");
                return 1;
            }
            break;
        case 'l':
            if (sscanf(optarg, "%d", &cfg.length) != 1) {
                fprintf(stderr, "Error: Option -l requires an integer argument.\n");
                return 1;
            }
            break;
        case 'g':
            if (sscanf(optarg, "%d", &cfg.group_size) != 1) {
                fprintf(stderr, "Error: Option -g requires an integer argument.\n");
                return 1;
            }
            break;
        case 'n':
            if (sscanf(optarg, "%d", &cfg.count) != 1) {
                fprintf(stderr, "Error: Option -n requires an integer argument.\n");
                return 1;
            }
            break;
        case 'd':
            cfg.dir = optarg;
            break;
        case 'f':
            cfg.format_str = optarg;
            break;
        default:
            fprintf(stderr, "Error: invalid arguments.\n");
            return 1;
        }
    }

    // Проверка, есть ли имя файла, либо директория
    if (cfg.file_name == NULL && cfg.dir == NULL) {
        fprintf(stderr, "Error: input file (-i) or directory (-d) must be specified.\n");
        return 1;
    }

    // Тест
    printf("File:        %s\n", cfg.file_name ? cfg.file_name : "(not set)");
    printf("Directory:   %s\n", cfg.dir ? cfg.dir : "(not set)");
    printf("Offset:      %d\n", cfg.offset);
    printf("Length:      %d\n", cfg.length);
    printf("Group size:  %d\n", cfg.group_size);
    printf("Count:       %d\n", cfg.count);
    printf("Format:      %s\n", cfg.format_str ? cfg.format_str : "(standard)");

    // ...

    return 0;
}