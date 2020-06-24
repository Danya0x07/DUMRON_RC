/*
 * Собственный itoa: ходят слухи, что _itoa из стандартной библиотеки
 * SDCC работает криво и ломает память.
 */
#define MAX_NUMBER_OF_DIGITS 16

char* itoa(int value, unsigned char radix)
{
    static char string[MAX_NUMBER_OF_DIGITS + 1];
    char buffer[MAX_NUMBER_OF_DIGITS + 1];
    const char hex_digit[] = "0123456789ABCDEF";
    char* pstr = string;
    char* pbuff = buffer;

    if (value < 0) {
        *pstr++ = '-';
        value = -value;
    }

    *pbuff++ = '\0';

    /* заполняем buffer строчным представлением числа в обратном порядке; */
    do {
        *pbuff++ = hex_digit[value % radix];
        value /= radix;
    } while (value != 0);

    /* копируем buffer в string задом наперёд. */
    do {
        *pstr++ = *--pbuff;
    } while (*pbuff != '\0');

    return string;
}