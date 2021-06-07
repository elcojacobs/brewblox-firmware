/*
 * Copyright 2020 BrewPi B.V./Elco Jacobs.
 *
 * This file is part of Brewblox.
 * 
 * Brewblox is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Brewblox is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with Brewblox.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <array>
#include <assert.h>
#include <atomic>
#include <mutex>
#include <optional>

/**
 * A ringBuffer implementation.
 * 
 * @tparam T The type of the elements to be saved.
 * @tparam n The amount of elements that can be maxmimum saved.
 */
template <typename T, unsigned int n>
class RingBuffer {
public:
    /**
     * Gives the user to a pointer to an empty T
     */
    T* take()
    {
        uint32_t oldTail;
        uint32_t newTail;
        T* allocatedElement;

        do {
            oldTail = this->tail;
            newTail = oldTail;
            if (isFull(head, oldTail)) {
                return nullptr;
            }

            allocatedElement = &buffer[newTail];
            if (newTail == n) {
                newTail = 0;
            } else {
                newTail++;
            }
        } while (!tail.compare_exchange_weak(oldTail, newTail, std::memory_order_relaxed));

        return allocatedElement;
    }

    /**
     * Frees the given element so it can be used again. This must be the oldest element still in use.
     * @param t The element to free.
     */
    void giveBack(T* t)
    {
        assert(t == &buffer[head]); // The pointer given must be the head pointer.
        if (head == n) {
            head = 0;
        } else {
            head++;
        }
    }

private:
    bool isFull(uint32_t head, uint32_t tail)
    {
        if (tail > head) {
            return (tail - head) == (n);
        } else {
            return (head - tail) == 1;
        }
    }
    std::mutex mutex;
    std::array<T, n + 1> buffer;
    std::atomic<uint32_t> head = 0;
    std::atomic<uint32_t> tail = 0;
};