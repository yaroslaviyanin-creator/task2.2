/*
test_lib.c - модуль проверки библиотеки.
Янин Ярослав Иванович
Группа МК-101
*/

#include "unity.h"
#include "lib_main.h"
#include <stdio.h>

// Имя временного файла для тестов
const char* TEMP_TEST_FILE = "temp_lib_test.bin";

void setUp(void) {
    // Создаем тестовый файл
    FILE* f = fopen(TEMP_TEST_FILE, "wb");
    if (f) {
        fputs("0123456789", f);
        fclose(f);
    }
}

void tearDown(void) {
    // Удаляем тестовый файл
    remove(TEMP_TEST_FILE);
}

// Тест 1: Проверка базовой структуры Config
void test_ConfigDefaults(void) {
    Config cfg = { 0 };
    cfg.length = -1;
    cfg.group_size = 1;

    TEST_ASSERT_EQUAL_INT(-1, cfg.length);
    TEST_ASSERT_EQUAL_INT(1, cfg.group_size);
    TEST_ASSERT_NULL(cfg.file_name);
}

// Тест 2: Защита от несуществующего файла
void test_ProcessFile_FileNotFound(void) {
    Config cfg = { 0 };
    process_file("Z:\\this_file_does_not_exist.bin", &cfg);
    TEST_PASS();
}

// Тест 3: Защита от слишком большого смещения (-o)
void test_ProcessFile_OffsetExceedsSize(void) {
    Config cfg = { 0 };
    cfg.offset = 1000;
    process_file(TEMP_TEST_FILE, &cfg);
    TEST_PASS();
}

// Тест 4: Защита от несуществующей директории
void test_ProcessDirectory_InvalidPath(void) {
    Config cfg = { 0 };
    process_directory("Z:\\invalid_folder_name_123", &cfg);
    TEST_PASS();
}

// Тест 5: Вызов кастомного формата без самой строки формата (NULL)
void test_PrintCustomFormat_NullString(void) {
    Config cfg = { 0 };
    cfg.format_str = NULL;
    unsigned char dummy_buf[] = { 0xFF };
    print_custom_format(dummy_buf, 1, 0, 0, &cfg);
    TEST_PASS();
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_ConfigDefaults);
    RUN_TEST(test_ProcessFile_FileNotFound);
    RUN_TEST(test_ProcessFile_OffsetExceedsSize);
    RUN_TEST(test_ProcessDirectory_InvalidPath);
    RUN_TEST(test_PrintCustomFormat_NullString);

    return UNITY_END();
}