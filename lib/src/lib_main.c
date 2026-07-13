/*
lib_main.c - главный модуль библиотеки.
Янин Ярослав Иванович
Группа МК-101
*/

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
