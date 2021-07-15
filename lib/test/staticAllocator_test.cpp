#include "staticAllocator.hpp"
#include <catch.hpp>

TEST_CASE("Elements can be taken from the StaticAllocator", "[StaticAllocator]")
{
    auto buffer = StaticAllocator<uint8_t, 5>();
    auto thing1 = buffer.get();
    REQUIRE(thing1);
    auto thing2 = buffer.get();
    REQUIRE(thing2);
    auto thing3 = buffer.get();
    REQUIRE(thing3);
    auto thing4 = buffer.get();
    REQUIRE(thing4);
    auto thing5 = buffer.get();
    REQUIRE(thing5);
}

TEST_CASE("Elements can be given back to the StaticAllocator", "[StaticAllocator]")
{
    auto buffer = StaticAllocator<uint8_t, 5>();
    auto thing1 = buffer.get();
    auto thing2 = buffer.get();
    auto thing3 = buffer.get();
    auto thing4 = buffer.get();
    auto thing5 = buffer.get();
    buffer.free(thing1);
    buffer.free(thing2);
    buffer.free(thing3);
    buffer.free(thing4);
    buffer.free(thing5);
}

TEST_CASE("A nullopt will be returned when an element is taken when the StaticAllocator is full", "[StaticAllocator]")
{
    auto buffer = StaticAllocator<uint8_t, 5>();
    auto thing1 = buffer.get();
    REQUIRE(thing1);
    auto thing2 = buffer.get();
    REQUIRE(thing2);
    auto thing3 = buffer.get();
    REQUIRE(thing3);
    auto thing4 = buffer.get();
    REQUIRE(thing4);
    auto thing5 = buffer.get();
    REQUIRE(thing5);
    auto thing6 = buffer.get();
    REQUIRE(!thing6);
}

TEST_CASE("Elements can be taken and added from the StaticAllocator", "[StaticAllocator]")
{
    auto buffer = StaticAllocator<uint8_t, 5>();
    auto thing1 = buffer.get();
    REQUIRE(thing1);
    auto thing2 = buffer.get();
    REQUIRE(thing2);
    auto thing3 = buffer.get();
    REQUIRE(thing3);
    auto thing4 = buffer.get();
    REQUIRE(thing4);
    auto thing5 = buffer.get();
    REQUIRE(thing5);
    buffer.free(thing1);
    auto thing6 = buffer.get();
    REQUIRE(thing6);
}

TEST_CASE("countFreeElements returns the right amount of free elements", "[StaticAllocator]")
{
    auto buffer = StaticAllocator<uint8_t, 5>();
    REQUIRE(buffer.countFreeElements() == 5);
    auto thing1 = buffer.get();
    REQUIRE(buffer.countFreeElements() == 4);
    auto thing2 = buffer.get();
    REQUIRE(buffer.countFreeElements() == 3);
    auto thing3 = buffer.get();
    REQUIRE(buffer.countFreeElements() == 2);
    auto thing4 = buffer.get();
    REQUIRE(buffer.countFreeElements() == 1);
    auto thing5 = buffer.get();
    REQUIRE(buffer.countFreeElements() == 0);
    buffer.free(thing1);
    REQUIRE(buffer.countFreeElements() == 1);
    auto thing6 = buffer.get();
    REQUIRE(buffer.countFreeElements() == 0);
}