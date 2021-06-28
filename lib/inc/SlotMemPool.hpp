
#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <cstddef>

/**
 * 
 * An allocator which uses static storage for storing the elements.
 * 
 * @tparam T The type of the elements to be stored.
 * @tparam n The amount of elements that can be stored.
 * 
 */
template <size_t maxElementSize, size_t n>
class SlotMemPool {
public:
    /**
     * Returns a pointer to an empty T.
     * 
     * @return A pointer to a T if space is available. If the buffer is full a nullptr will be returned.
     * 
     */
    template <typename T>
    [[nodiscard]] T* get()
    {
        static_assert(sizeof(T) <= maxElementSize, "The element that is requested is bigger than the slot size.");
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
        return reinterpret_cast<T*>(&element->data);
    }

    /**
     * Frees an element in the buffer.
     * 
     * @param element The element to be freed.
     * 
     */
    template <typename T>
    void free(T* element)
    {
        assert(tryFree(element));
    }

    /**
     * Tries to free the element. If it's not in the buffer nothing will happen. 
     * This is usefull if you're not sure if the element needs to be freed.
     * 
     * @param element The element to be freed.
     * @return Returns true if the element was in the buffer, false if it was not.
     * 
     */
    template <typename T>
    bool tryFree(T* element)
    {
        auto index = std::find_if(
            data.begin(),
            data.end(),
            [element](Element& ele) {
                return reinterpret_cast<T*>(&ele.data) == element;
            });

        if (index != data.end()) {
            assert(index->inUse);
            index->inUse = false;
            return true;
        }
        return false;
    }

    /**
     * Returns the amount of elements in the buffer which are still free. 
     */
    size_t
    countFreeElements()
    {
        return std::count_if(data.begin(), data.end(), [](const Element& ele) {
            return !ele.inUse;
        });
    }

private:
    struct Element {
        uint8_t data[maxElementSize];
        std::atomic<bool> inUse = false;
    };
    std::array<Element, n> data;
};
