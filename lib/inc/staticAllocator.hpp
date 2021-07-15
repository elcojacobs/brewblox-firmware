#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cstddef>

/**
 * 
 * An alloctor which uses static storage for storing the elements.
 * 
 * @tparam T The type of the elements to be stored.
 * @tparam size The amount of elements that can be stored.
 * 
 */
template <typename T, size_t size>
class StaticAllocator {
public:
    /**
     * Returns a pointer to an empty T.
     * 
     * @return A pointer to a T if space is available. If the buffer is full a nullptr will be returned.
     * 
     */
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

    /**
     * Frees an element in the buffer.
     * 
     * @param element The element to be freed.
     * 
     */
    void free(T* element)
    {
        auto index = std::find_if(data.begin(), data.end(), [element](Element& ele) { return &ele.data == element; });
        assert(index->inUse);
        index->inUse = false;
    }

    /**
     * Returns the amount of elements in the buffer which are still free. 
     */
    size_t countFreeElements()
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
