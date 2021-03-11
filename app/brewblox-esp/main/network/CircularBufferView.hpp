#pragma once

#include "CircularBuffer.hpp"

// Just a non-owning proxy for CircularBuffer class instances
template <std::size_t Capacity>
class CircularBufferView {
public:
    using buffer_type = CircularBuffer<Capacity>;
    using const_buffers_type = typename buffer_type::const_buffers_type;
    using mutable_buffers_type = typename buffer_type::mutable_buffers_type;

    CircularBufferView(buffer_type& buffer)
        : buffer(buffer)
    {
    }

    auto prepare(std::size_t n)
    {
        return buffer.prepare(n);
    }

    void commit(std::size_t n)
    {
        buffer.commit(n);
    }

    void consume(std::size_t n)
    {
        buffer.consume(n);
    }

    auto data() const
    {
        return buffer.data();
    }

    auto size() const
    {
        return buffer.size();
    }

    constexpr auto max_size() const
    {
        return buffer.max_size();
    }

    constexpr auto capacity() const
    {
        return buffer.capacity();
    }

private:
    buffer_type& buffer;
};

template <std::size_t Capacity>
CircularBufferView<Capacity>
make_view(CircularBuffer<Capacity>& buffer)
{
    return buffer;
}