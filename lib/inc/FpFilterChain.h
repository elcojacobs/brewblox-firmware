/*
 * Copyright 2018 BrewPi/Elco Jacobs.
 *
 * This file is part of BrewPi.
 *
 * BrewPi is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * BrewPi is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with BrewPi.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once
#include "FilterChain.h"
#include "FixedPoint.h"
#include <type_traits>

template <typename T>
class FpFilterChain {
private:
    FilterChain chain;
    uint8_t readIdx; // 0 for no filtering, 1 - 6 for each filter stage

public:
    using value_type = T;

    FpFilterChain(uint8_t idx, uint8_t initStages = 0)
        : chain({0, 2, 2, 2, 2, 2}, {2, 2, 2, 3, 3, 4}, initStages)
    {
        setReadIdx(idx);
    }
    FpFilterChain(const FpFilterChain&) = delete;
    FpFilterChain& operator=(const FpFilterChain&) = delete;
    ~FpFilterChain() = default;

    void add(value_type val)
    {
        chain.add(cnl::unwrap(val));
    }
    void add(int32_t val);

    void setReadIdx(uint8_t idx)
    {
        readIdx = idx;
        expandStages(readIdx);
    }

    uint8_t getReadIdx() const
    {
        return readIdx;
    }

    void setStepThreshold(value_type stepThreshold)
    {
        chain.setStepThreshold(cnl::unwrap(stepThreshold));
    }
    value_type getStepThreshold() const
    {
        return cnl::wrap<value_type>(chain.getStepThreshold());
    }
    value_type read() const
    {
        if (readIdx == 0) {
            return cnl::wrap<value_type>(chain.readLastInput());
        }
        return cnl::wrap<value_type>(chain.read(readIdx - 1));
    }

    value_type read(uint8_t filterNr) const
    {
        return cnl::wrap<value_type>(chain.read(filterNr));
    }

    value_type readLastInput() const
    {
        return cnl::wrap<value_type>(chain.readLastInput());
    }

    uint8_t length() const
    {
        return chain.length();
    }

    // get the derivative from the chain with max precision and convert to the requested FP precision
    template <typename U>
    U readDerivative(uint8_t idx) const
    {
        auto derivative = chain.readDerivative(idx);
        uint8_t destFractionBits = cnl::_impl::fractional_digits<U>::value;
        uint8_t filterFactionBits = cnl::_impl::fractional_digits<T>::value + derivative.fractionBits;
        int64_t result;
        if (destFractionBits >= filterFactionBits) {
            result = derivative.result << (destFractionBits - filterFactionBits);
        } else {
            result = derivative.result >> (filterFactionBits - destFractionBits);
        }
        return cnl::wrap<U>(result);
    }

    template <typename U>
    U readDerivative() const
    {
        return readDerivative<U>(readIdx > 0 ? readIdx - 1 : 0);
    }

    auto intervalToFilterNr(uint16_t maxInterval)
    {
        return chain.intervalToFilterNr(maxInterval);
    }

    void reset(value_type value)
    {
        chain.reset(cnl::unwrap(value));
    }

    void expandStages(size_t numStages)
    {
        chain.expandStages(numStages);
    }
};
