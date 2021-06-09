#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cstddef>

template <typename T, size_t size>
class StaticAllocator {
public:
    [[nodiscard]] T* get()
    {
        Element* element;

        bool expected = false;

        do {
            expected = false;
            element = std::find_if(data.begin(), data.end(), [](const Element& ele) {
                return !ele.inUse;
            });
            if (element == data.end()) {
                return nullptr;
            }
        } while (!element->inUse.compare_exchange_weak(expected, true));
        return &element->data;
    }

    void free(T* element)
    {
        auto index = std::find_if(data.begin(), data.end(), [element](Element& ele) { return &ele == reinterpret_cast<StaticAllocator<T, size>::Element*>(element); });
        assert(index->inUse);
        index->inUse = false;
    }

    uint32_t countFreeElements()
    {
        return std::count_if(data.begin(), data.end(), [](const Element& ele) {
            return !ele.inUse;
        });
    }

private:
    struct Element {
        T data;
        std::atomic<bool> inUse = false;
    };
    std::array<Element, size> data;
};
