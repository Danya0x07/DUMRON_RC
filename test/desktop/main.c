#include <unity.h>

extern void test_delay(void);
extern void test_draw(void);
extern void test_find_clear_rf_ch(void);

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_delay);
    RUN_TEST(test_draw);
    RUN_TEST(test_find_clear_rf_ch);

    return UNITY_END();
}