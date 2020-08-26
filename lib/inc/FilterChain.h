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
#include "IirFilter.h"
#include <cstdint>
#include <limits>
#include <memory>
#include <vector>

class FilterChain {
private:
    struct Stage {
        std::unique_ptr<IirFilter> filter;
        uint8_t param;
        uint8_t interval;
    };
    std::vector<Stage> stages;

    uint32_t counter = 0;

public:
    FilterChain(std::vector<uint8_t> params);
    FilterChain(std::vector<uint8_t> params, std::vector<uint8_t> intervals, uint8_t initStages = 0, int32_t stepThreshold = std::numeric_limits<int32_t>::max());

    FilterChain(const FilterChain&) = delete;
    FilterChain(FilterChain&&) = default;
    FilterChain& operator=(const FilterChain&) = delete;

    ~FilterChain() = default;

    void add(int32_t val);
    void expandStages(size_t numStages);
    void setStepThreshold(int32_t threshold);                       // set the step detection threshold
    int32_t getStepThreshold() const;                               // get the step detection threshold of last filter
    int32_t read(uint8_t filterNr = 255, bool smooth = true) const; // read from specified filter, default to last
    uint32_t sampleInterval(uint8_t filterNr = 255) const;          // get minimum sample interval of filter at index i
    uint8_t intervalToFilterNr(uint32_t maxInterval) const;         // get slowest filter number with interval faster than argument

    uint32_t getCount() const
    {
        return counter;
    } // return count. Can be used to synchronize sensor switching
    uint32_t getCount(uint8_t filterNr) const
    {
        return counter / sampleInterval(filterNr - 1);
    } // return count for a specific filter
    uint8_t length() const
    {
        return selectStage(stages.size() - 1) - stages.cbegin() + 1;
    }

    int64_t readWithNFractionBits(uint8_t bits, uint8_t filterNr = 255) const;
    int32_t readLastInput() const;
    IirFilter::DerivativeResult readDerivative(uint8_t filterNr, bool smooth = true) const;

    void reset(int32_t value);

private:
    uint32_t sampleInterval(std::vector<Stage>::const_iterator stage) const;
    std::vector<FilterChain::Stage>::const_iterator selectStage(uint8_t filterNr) const;
};
