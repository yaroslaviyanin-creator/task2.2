/*
lib_main.c - главный модуль библиотеки.
Янин Ярослав Иванович
Группа МК-101
*/

#include "lib_main.h"

// Функция для вывода в шестнадцатеричном виде
// <chunk> - указатель на буфер
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
// <buffer> - указатель на массив считанных байтов
// <bytes_read> - количество реально считанных байтов, находящихся в буфере
// <current_offset> - текущее смещение от начала файла
// <cfg> - указатель на структуру с аргументамми
void print_standard_format(const unsigned char* buffer, int bytes_read, int current_offset, Config* cfg) {
    // Выводим текущее смещение
    printf("%08X  ", current_offset);

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


// Функция для обхода директории
// <dirpath> - путь к папке
// <cfg> - указатель на структуру с аргументамми
void process_directory(const char* dirpath, Config* cfg) {
    WIN32_FIND_DATAA findFileData;

    char search_path[1024];
    // Добавляем "\*" для поиска в директории, и сохраняем адрес в search_path
    snprintf(search_path, sizeof(search_path), "%s\\*", dirpath);

    // Ищем первый файл и сохраняем его номер в hFind
    HANDLE hFind = FindFirstFileA(search_path, &findFileData);
    if (hFind == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Error: Cannot open directory '%s'\n", dirpath);
        return;
    }

    // Проходим по всем файлам
    do {
        // Пропускаем системные папки "." и ".."
        if (strcmp(findFileData.cFileName, ".") == 0 || strcmp(findFileData.cFileName, "..") == 0) {
            continue;
        }

        // Пропускаем вложенные папки
        if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            continue;
        }

        char filepath[1024];
        // Создаём путь до файла и сохраняем в filepath
        snprintf(filepath, sizeof(filepath), "%s\\%s", dirpath, findFileData.cFileName); 

        // Защита от дубликатов, приводим все относительные пути в абсолютным
        if (cfg->file_name != NULL) {
            char abs_path_cfg[MAX_PATH];
            char abs_path_dir[MAX_PATH];
            // Перевод в абсолютный путь
            GetFullPathNameA(cfg->file_name, MAX_PATH, abs_path_cfg, NULL);
            GetFullPathNameA(filepath, MAX_PATH, abs_path_dir, NULL);

            // Сравниваем строки без учета регистра при помощи _stricmp
            if (_stricmp(abs_path_cfg, abs_path_dir) == 0) {
                continue; // Если пути указывают на один и тот же файл то не выводим его второй раз
            }
        }

        printf("\n*File: %s\n", filepath);
        // Открытие файла
        process_file(filepath, cfg);

    } while (FindNextFileA(hFind, &findFileData) != 0); // Ищем следующий файл

    // Закрываем дескриптор поиска
    FindClose(hFind);
}


// Функция для вывода данных по форматной строке
// <buffer> - указатель на массив считанных байтов
// <bytes_read> - количество реально считанных байтов, находящихся в буфере
// <current_offset> - текущее смещение от начала файла
// <line_index> - индекс текущей строки, начиная с 0
// <cfg> - указатель на структуру с аргументамми
void print_custom_format(const unsigned char* buffer, int bytes_read, int current_offset, int line_index, Config* cfg) {
    if (cfg->format_str == NULL) return;
    const char* fmt = cfg->format_str;

    while (*fmt != '\0') {
        if (*fmt == '\\') {
            fmt++; // Смотрим следующий символ за слэшем
            // Обработка спецсимволов (\n, \t, \r, \\)
            if (*fmt == 'n'){ 
                printf("\n"); 
            }
            else if (*fmt == 't'){ 
                printf("\t"); 
            }
            else if (*fmt == 'r'){ 
                printf("\r"); 
            }
            else if (*fmt == '\\'){ 
                printf("\\"); 
            }
            else{ 
                printf("%c", *fmt); 
            }
            fmt++; // Смотрим следующий символ
        }

        else if (*fmt == '%') {
            fmt++; // Смотрим следующий символ за процента
            if (*fmt == 'i') {
                printf("%d", line_index);
                fmt++;
            }
            else if (*fmt == 'n') {
                printf("%08X", current_offset);
                fmt++;
            }
            else if (isdigit((unsigned char)*fmt)) { // isdigit - проверка, является ли символ цифрой
                // Если после % идет цифра, значит это спецификатор группы
                int chunk_index = 0;                 // Спецификатор группы

                // Считываем числа(символы) пока они не закончатся
                while (isdigit((unsigned char)*fmt)) {
                    chunk_index = chunk_index * 10 + (*fmt - '0'); // Превращаем символы в число
                    fmt++;
                }
                char type = *fmt;           // type - типо вывода, а fmt указывает на него в нашей строке
                if (type != '\0') fmt++;    // Проверка конца
                int start_byte = chunk_index * cfg->group_size; // Байт, с которого начинается нужная нам часть

                // Проверяем, есть ли вообще такие данные в прочитанном буфере
                if (start_byte < bytes_read) {
                    int chunk_bytes = bytes_read - start_byte; // Кол-во доступных нам байт в буфере
                    if (chunk_bytes > cfg->group_size) {
                        chunk_bytes = cfg->group_size;
                    }

                    // Выводим в нужном формате
                    if (type == 'x') {
                        // Используем функцию вывода HEX
                        print_hex_group(&buffer[start_byte], chunk_bytes, cfg->group_size);
                    }
                    else if (type == 'c') {
                        // Выводим как ASCII символы
                        for (int j = 0; j < chunk_bytes; j++) {
                            unsigned char c = buffer[start_byte + j];
                            if (c >= 32 && c <= 126) printf("%c", c);
                            else printf(".");
                        }
                    }
                }
                else {
                    // Если данных нет (конец файла), выводим пробелы для сохранения выравнивания
                    if (type == 'x') {
                        for (int j = 0; j < cfg->group_size * 2; j++) printf(" ");
                    }
                    else if (type == 'c') {
                        for (int j = 0; j < cfg->group_size; j++) printf(" ");
                    }
                }
            }
            else {
                // Если после % идет еще один %, просто печатаем % и перешагиваем
                printf("%%");
                if (*fmt == '%') fmt++;
            }
        }
        else {
            // Если обычный символ, то просто выводим
            printf("%c", *fmt);
            fmt++;
        }
    }
}


// Функция для обработки и побайтового вывода одного файла
// <filepath> - путь к файлу, который необходимо открыть и прочитать
// <cfg> - указатель на структуру с настройками
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
    fseek(file, cfg->offset, SEEK_SET);


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
    int line_index = 0;                 // Счетчик строк

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

        // Проверка форматной строки
        if (cfg->format_str == NULL) {
            // Стандартный вывод
            print_standard_format(buffer, bytes_read, current_offset, cfg);
        }
        else {
            // Кастомный вывод
            print_custom_format(buffer, bytes_read, current_offset, line_index, cfg);
        }


        // Обновляем счетчики
        total_read += bytes_read;
        current_offset += bytes_read;
        line_index++;
    }

    // Очищаем память и закрываем файл
    free(buffer);
    fclose(file);
}
