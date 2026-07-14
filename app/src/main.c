/*
main.c - главный модуль программы.
Янин Ярослав Иванович
Группа МК-101
*/

#include <stdio.h>
#include <stdlib.h>
#include "getopt.h"
#include "config.h"
#include "lib_main.h"

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
            if (sscanf(optarg, "%llu", &cfg.offset) != 1) {
                fprintf(stderr, "Error: Option -o requires an integer argument.\n");
                return 1;
            }
            break;
        case 'l':
            if (sscanf(optarg, "%lld", &cfg.length) != 1) {
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

    // Тесты
    // Тест считывания аргументов
    printf("File:        %s\n", cfg.file_name ? cfg.file_name : "(not set)");
    printf("Directory:   %s\n", cfg.dir ? cfg.dir : "(not set)");
    printf("Offset:      %d\n", cfg.offset);
    printf("Length:      %d\n", cfg.length);
    printf("Group size:  %d\n", cfg.group_size);
    printf("Count:       %d\n", cfg.count);
    printf("Format:      %s\n", cfg.format_str ? cfg.format_str : "(standard)");


    // Обрабатываем одиночный файл
    if (cfg.file_name != NULL) {
        printf("\n*File: %s\n", cfg.file_name);
        process_file(cfg.file_name, &cfg);
    }

    // Обрабатываем директорию
    if (cfg.dir != NULL) {
        process_directory(cfg.dir, &cfg);
    }

    // ...

    return 0;
}