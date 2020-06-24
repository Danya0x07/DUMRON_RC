#include <unity.h>

extern void test_delay(void);

int main(void)
{
    UNITY_BEGIN();

    RUN_TEST(test_delay);

    return UNITY_END();
}