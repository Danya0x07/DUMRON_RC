#include <unity.h>

extern void test_delay(void);
extern void test_draw(void);

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_delay);
    RUN_TEST(test_draw);

    return UNITY_END();
}