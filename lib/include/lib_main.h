/*
lib_main.h - заголовочный файл модуля lib_main.c.
Янин Ярослав Иванович
Группа МК-101
*/
#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <windows.h>
#include <string.h>
#include "config.h"

// Функция для вывода в шестнадцатеричном виде
// <chunk> - Указатель на буфер
// <bytes_available> - кол-во байтов которые мы считали из файла
// <group_size> - размер буфера
void print_hex_group(const unsigned char* chunk, int bytes_available, int group_size);

// Функция для вывод на экран
// <buffer> - указатель на массив считанных байтов
// <bytes_read> - количество реально считанных байтов, находящихся в буфере
// <current_offset> - текущее смещение от начала файла
// <cfg> - указатель на структуру с аргументамми
void print_standard_format(const unsigned char* buffer, int bytes_read, int current_offset, Config* cfg);


// Функция для обхода директории
// <dirpath> - путь к папке
// <cfg> - указатель на структуру с настройками
void process_directory(const char* dirpath, Config* cfg);


// Функция для обработки и побайтового вывода одного файла
// <filepath> - путь к файлу, который необходимо открыть и прочитать
// <cfg> - указатель на структуру с настройками
void process_file(const char* filepath, Config* cfg);

// Функция для вывода данных по форматной строке
// <buffer> - указатель на массив считанных байтов
// <bytes_read> - количество реально считанных байтов, находящихся в буфере
// <current_offset> - текущее смещение от начала файла
// <line_index> - индекс текущей строки, начиная с 0
// <cfg> - указатель на структуру с аргументамми
void print_custom_format(const unsigned char* buffer, int bytes_read, int current_offset, int line_index, Config* cfg);
