#include "ringBuffer.hpp"
#include <catch.hpp>

TEST_CASE("Elements can be taken from the ringbuffer", "[ringBuffer]")
{
    auto buffer = RingBuffer<uint8_t, 5>();
    auto thing1 = buffer.take();
    REQUIRE(thing1);
    auto thing2 = buffer.take();
    REQUIRE(thing2);
    auto thing3 = buffer.take();
    REQUIRE(thing3);
    auto thing4 = buffer.take();
    REQUIRE(thing4);
    auto thing5 = buffer.take();
    REQUIRE(thing5);
}

TEST_CASE("Elements can be given back to the ringbuffer", "[ringBuffer]")
{
    auto buffer = RingBuffer<uint8_t, 5>();
    auto thing1 = buffer.take();
    auto thing2 = buffer.take();
    auto thing3 = buffer.take();
    auto thing4 = buffer.take();
    auto thing5 = buffer.take();
    buffer.giveBack(thing1.value());
    buffer.giveBack(thing2.value());
    buffer.giveBack(thing3.value());
    buffer.giveBack(thing4.value());
    buffer.giveBack(thing5.value());
}

TEST_CASE("A nullopt will be returned when an element is taken when the buffer is full", "[ringBuffer]")
{
    auto buffer = RingBuffer<uint8_t, 5>();
    auto thing1 = buffer.take();
    REQUIRE(thing1);
    auto thing2 = buffer.take();
    REQUIRE(thing2);
    auto thing3 = buffer.take();
    REQUIRE(thing3);
    auto thing4 = buffer.take();
    REQUIRE(thing4);
    auto thing5 = buffer.take();
    REQUIRE(thing5);
    auto thing6 = buffer.take();
    REQUIRE(!thing6);
}

TEST_CASE("Elements can be taken and added", "[ringBuffer]")
{
    auto buffer = ringBuffer<uint8_t, 5>();
    auto thing1 = buffer.take();
    REQUIRE(thing1);
    auto thing2 = buffer.take();
    REQUIRE(thing2);
    auto thing3 = buffer.take();
    REQUIRE(thing3);
    auto thing4 = buffer.take();
    REQUIRE(thing4);
    auto thing5 = buffer.take();
    REQUIRE(thing5);
    buffer.giveBack(thing1.value());
    auto thing6 = buffer.take();
    REQUIRE(thing6);
}