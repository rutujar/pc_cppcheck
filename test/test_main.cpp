#include <catch/catch.hpp>

int unittest_main(int argc, char *argv[]);

TEST_CASE("main: success")
{
    CHECK_NOTHROW(unittest_main(0, nullptr));
}
