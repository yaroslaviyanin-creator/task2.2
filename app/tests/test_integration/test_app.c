/*
test_app.c - модуль проверки программы.
Янин Ярослав Иванович
Группа МК-101
*/

#include "unity.h"
#include <stdlib.h>
#include <stdio.h>

const char* TEMP_APP_FILE = "temp_app_test.bin";

// ФУНКЦИЯ-РАДАР: Сама находит правильный путь до программы
const char* get_exe() {
    // Проверяем 3 самых частых варианта папок, откуда CTest может запускать тест
    if (fopen("..\\..\\Debug\\my_program.exe", "r")) return "..\\..\\Debug\\my_program.exe";
    if (fopen("..\\Debug\\my_program.exe", "r"))     return "..\\Debug\\my_program.exe";
    if (fopen("app\\Debug\\my_program.exe", "r"))    return "app\\Debug\\my_program.exe";

    // Если ничего не подошло (что вряд ли), возвращаем жесткий абсолютный путь
    return "G:\\WORK\\YAROSLAV\\Универ\\Практика\\task2.2\\build\\app\\Debug\\my_program.exe";
}

void setUp(void) {
    FILE* f = fopen(TEMP_APP_FILE, "wb");
    if (f) {
        fputs("Integration Testing!", f);
        fclose(f);
    }
}

void tearDown(void) {
    remove(TEMP_APP_FILE);
}

void test_App_InvalidFlag(void) {
    char cmd[512];
    sprintf(cmd, "%s -z > nul 2>&1", get_exe());
    int result = system(cmd);
    TEST_ASSERT_NOT_EQUAL(0, result);
}

void test_App_ValidFile(void) {
    char cmd[512];
    sprintf(cmd, "%s -i %s > nul 2>&1", get_exe(), TEMP_APP_FILE);
    int result = system(cmd);
    TEST_ASSERT_EQUAL(0, result);
}

void test_App_ComplexArguments(void) {
    char cmd[512];
    // Важно: в sprintf знак процента пишется дважды (%%), а слэш экранируется (\\n)
    sprintf(cmd, "%s -i %s -o 2 -l 5 -g 2 -n 4 -f \"Test: %%0x\\n\" > nul 2>&1", get_exe(), TEMP_APP_FILE);
    int result = system(cmd);
    TEST_ASSERT_EQUAL(0, result);
}

void test_App_WrongArgumentType(void) {
    char cmd[512];
    sprintf(cmd, "%s -i %s -o A > nul 2>&1", get_exe(), TEMP_APP_FILE);
    int result = system(cmd);
    TEST_ASSERT_NOT_EQUAL(0, result);
}

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_App_InvalidFlag);
    RUN_TEST(test_App_ValidFile);
    RUN_TEST(test_App_ComplexArguments);
    RUN_TEST(test_App_WrongArgumentType);

    return UNITY_END();
}