#include <catch/catch.hpp>
#include <producer.h>

#include <iostream>

TEST_CASE("producer: constructor / destructor")
{
    CHECK_NOTHROW(producer{});
}

TEST_CASE("producer: print msg")
{
    producer p{};
    CHECK_NOTHROW(p.print_msg());
}
