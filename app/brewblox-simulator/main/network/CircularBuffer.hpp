#pragma once
#include "esp_log.h"
#include <asio.hpp>

template <std::size_t Capacity>
class CircularBuffer : public std::streambuf {
public:
    using const_buffers_type = std::vector<asio::const_buffer>;
    using mutable_buffers_type = std::vector<asio::mutable_buffer>;

    auto prepare(std::size_t n)
    {
        if (size() + n > max_size()) {
            ESP_LOGE("CB", "CircularBuffer overflow");
            // return buffer of available free space instead of requested amount
            make_sequence<mutable_buffers_type>(buffer, tail, tail + free_space());
        }

        ESP_LOGI("CB", "prepare size %u buf size %u in_avail %u", n, size(), in_avail());

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
        return Capacity - 1; // prevent head/tail collision with -1
    }

    constexpr std::size_t capacity() const
    {
        return max_size();
    }

    auto free_space() const
    {
        return max_size() - size();
    }

    virtual int sync() override
    {
        if (on_sync) {
            return on_sync();
        }
        return 0;
    }

    void set_sync_func(std::function<int()>&& func)
    {
        on_sync = func;
    }

    int peek()
    {
        if (size()) {
            return buffer[head];
        }
        return -1;
    }

    virtual int showmanyc() override
    {
        return size();
    }

    // int get()
    // {
    //     if (size()) {
    //         auto v = buffer[head];
    //         consume(1);
    //         return v;
    //     }
    //     return -1;
    // }

    // void put(char v)
    // {
    //     if (free_space()) {
    //         buffer[tail] = v;
    //         commit(1);
    //     }
    // }

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
    std::function<int()> on_sync;
};