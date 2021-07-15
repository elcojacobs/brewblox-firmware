#include "maxSizeStaticAllocator.hpp"
#include "staticAllocator.hpp"
#include <catch.hpp>

TEST_CASE("Elements can be taken from the MaxSizeStaticAllocator", "[MaxSizeStaticAllocator]")
{
    auto buffer = MaxSizeStaticAllocator<1, 5>();
    auto thing1 = buffer.get<uint8_t>();
    REQUIRE(thing1);
    auto thing2 = buffer.get<uint8_t>();
    REQUIRE(thing2);
    auto thing3 = buffer.get<uint8_t>();
    REQUIRE(thing3);
    auto thing4 = buffer.get<uint8_t>();
    REQUIRE(thing4);
    auto thing5 = buffer.get<uint8_t>();
    REQUIRE(thing5);
}

TEST_CASE("Elements can be given back to the MaxSizeStaticAllocator", "[MaxSizeStaticAllocator]")
{
    auto buffer = MaxSizeStaticAllocator<1, 5>();
    auto thing1 = buffer.get<uint8_t>();
    auto thing2 = buffer.get<uint8_t>();
    auto thing3 = buffer.get<uint8_t>();
    auto thing4 = buffer.get<uint8_t>();
    auto thing5 = buffer.get<uint8_t>();
    buffer.free(thing1);
    buffer.free(thing2);
    buffer.free(thing3);
    buffer.free(thing4);
    buffer.free(thing5);
}

TEST_CASE("A nullopt will be returned when an element is taken when the MaxSizeStaticAllocator is full", "[MaxSizeStaticAllocator]")
{
    auto buffer = MaxSizeStaticAllocator<1, 5>();
    auto thing1 = buffer.get<uint8_t>();
    REQUIRE(thing1);
    auto thing2 = buffer.get<uint8_t>();
    REQUIRE(thing2);
    auto thing3 = buffer.get<uint8_t>();
    REQUIRE(thing3);
    auto thing4 = buffer.get<uint8_t>();
    REQUIRE(thing4);
    auto thing5 = buffer.get<uint8_t>();
    REQUIRE(thing5);
    auto thing6 = buffer.get<uint8_t>();
    REQUIRE(!thing6);
}

TEST_CASE("Elements can be taken and added from the MaxSizeStaticAllocator", "[MaxSizeStaticAllocator]")
{
    auto buffer = MaxSizeStaticAllocator<1, 5>();
    auto thing1 = buffer.get<uint8_t>();
    REQUIRE(thing1);
    auto thing2 = buffer.get<uint8_t>();
    REQUIRE(thing2);
    auto thing3 = buffer.get<uint8_t>();
    REQUIRE(thing3);
    auto thing4 = buffer.get<uint8_t>();
    REQUIRE(thing4);
    auto thing5 = buffer.get<uint8_t>();
    REQUIRE(thing5);
    buffer.free(thing1);
    auto thing6 = buffer.get<uint8_t>();
    REQUIRE(thing6);
}

TEST_CASE("countFreeElements returns the right amount of free elements for the MaxSizeStaticAllocator", "[MaxSizeStaticAllocator]")
{
    auto buffer = MaxSizeStaticAllocator<1, 5>();
    REQUIRE(buffer.countFreeElements() == 5);
    auto thing1 = buffer.get<uint8_t>();
    REQUIRE(buffer.countFreeElements() == 4);
    auto thing2 = buffer.get<uint8_t>();
    REQUIRE(buffer.countFreeElements() == 3);
    auto thing3 = buffer.get<uint8_t>();
    REQUIRE(buffer.countFreeElements() == 2);
    auto thing4 = buffer.get<uint8_t>();
    REQUIRE(buffer.countFreeElements() == 1);
    auto thing5 = buffer.get<uint8_t>();
    REQUIRE(buffer.countFreeElements() == 0);
    buffer.free(thing1);
    REQUIRE(buffer.countFreeElements() == 1);
    auto thing6 = buffer.get<uint8_t>();
    REQUIRE(buffer.countFreeElements() == 0);
}

TEST_CASE("countFreeElements can handle different sizes", "[MaxSizeStaticAllocator]")
{
    auto buffer = MaxSizeStaticAllocator<4, 6>();
    auto thing1 = buffer.get<uint8_t>();
    auto thing2 = buffer.get<uint16_t>();
    auto thing3 = buffer.get<uint32_t>();
    auto thing4 = buffer.get<uint8_t>();
    auto thing5 = buffer.get<uint16_t>();
    auto thing6 = buffer.get<uint32_t>();

    buffer.free(thing1);
    buffer.free(thing2);
    buffer.free(thing3);
    buffer.free(thing4);
    buffer.free(thing5);
}