#pragma once

#include "circular_buffer.hpp"

// Just a non-owning proxy for circular_buffer class instances
template <std::size_t Capacity>
class circular_buffer_view {
public:
    using buffer_type = circular_buffer<Capacity>;
    using const_buffers_type = typename buffer_type::const_buffers_type;
    using mutable_buffers_type = typename buffer_type::mutable_buffers_type;

    circular_buffer_view(buffer_type& buffer)
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
circular_buffer_view<Capacity>
make_view(circular_buffer<Capacity>& buffer)
{
    return buffer;
}