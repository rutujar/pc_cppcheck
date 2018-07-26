#include <hippomocks.h>
#include <catch/catch.hpp>

#include <consumer.h>

#ifdef _HIPPOMOCKS__ENABLE_CFUNC_MOCKING_SUPPORT

TEST_CASE("consumer: failure")
{
    MockRepository mocks;
    auto p = mocks.Mock<producer>();

    mocks.OnCall(p, producer::print_msg).Throw(std::runtime_error("error"));

    CHECK_THROWS(consumer{p});
}

TEST_CASE("consumer: success")
{
    MockRepository mocks;
    auto p = mocks.Mock<producer>();

    mocks.OnCall(p, producer::print_msg);

    CHECK_NOTHROW(consumer{p});
}

#endif
