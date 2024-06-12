#include <unity.h>
#include <Foo.h>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_is_foo_false_after_init() {
    Foo foo;

    TEST_ASSERT_FALSE(foo.isFoo);
}

int main( int argc, char **argv) {
    UNITY_BEGIN();
    RUN_TEST(test_is_foo_false_after_init);
    UNITY_END();
}