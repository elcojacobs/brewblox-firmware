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
    std::optional<T*> take()
    {
        const std::lock_guard<std::mutex> lock(mutex);

        if (isFull) {
            return std::nullopt;
        }
        unsigned int toReturn = tail;
        if (tail == n - 1) {
            tail = 0;
        } else {
            tail++;
        }

        if (head == tail)
            isFull = true;

        return &buffer[toReturn];
    }

    /**
     * Frees the given element so it can be used again. This must be the oldest element still in use.
     * @param t The element to free.
     */
    void giveBack(T* t)
    {
        const std::lock_guard<std::mutex> lock(mutex);
        if (isFull)
            isFull = false;

        assert(t == &buffer[head]); // The pointer given must be the head pointer.

        if (head == n - 1) {
            head = 0;
        } else {
            head++;
        }
    }

    /**
     * Returns the amount of elements that are free for taking.
     */
    unsigned int getFreeSpace()
    {
        const std::lock_guard<std::mutex> lock(mutex);
        if (isFull) {
            return 0;
        }
        if (head == tail) {
            return n;
        }
        if (tail > head) {
            return tail - head;
        }
        return head - tail;
    }

private:
    std::mutex mutex;
    bool isFull = false;
    std::array<T, n> buffer;
    std::atomic<unsigned int> head = 0;
    std::atomic<unsigned int> tail = 0;
};