#pragma once
#include <asio.hpp>

template <std::size_t Capacity>
class circular_buffer {
public:
    using const_buffers_type = std::vector<asio::const_buffer>;
    using mutable_buffers_type = std::vector<asio::mutable_buffer>;

    auto prepare(std::size_t n)
    {
        // Boost.Asio Dynamic buffer throws std::length_error in this case, so we'll do the same
        if (size() + n > max_size()) {
            throw std::length_error("circular_buffer overflow");
        }

        return make_sequence<mutable_buffers_type>(buffer, tail, tail + n);
    }

    void commit(std::size_t n)
    {
        tail += n;
    }

    void consume(std::size_t n)
    {
        head += n;
    }

    auto data() const
    {
        return make_sequence<const_buffers_type>(buffer, head, tail);
    }

    std::size_t size() const
    {
        return tail - head;
    }

    constexpr std::size_t max_size() const
    {
        return Capacity;
    }

    constexpr std::size_t capacity() const
    {
        return max_size();
    }

private:
    // Static template function is a trick to cover both const and non-const variants using the same function
    template <typename Sequence, typename Buffer>
    static Sequence make_sequence(Buffer& buffer, std::size_t begin, std::size_t end)
    {
        std::size_t size = end - begin;
        begin %= Capacity;
        end %= Capacity;

        if (begin <= end) // Continuous flat case
        {
            return {
                typename Sequence::value_type(&buffer[begin], size)};
        } else // Looped case
        {
            std::size_t ending = Capacity - begin;

            return {
                typename Sequence::value_type(&buffer[begin], ending),
                typename Sequence::value_type(&buffer[0], size - ending)};
        }
    }

    std::array<char, Capacity> buffer;
    std::size_t head = 0;
    std::size_t tail = 0;
};