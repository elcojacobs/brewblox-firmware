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

#include <FilterChain.h>
#include <limits>
#include <memory>

FilterChain::FilterChain(
    std::vector<uint8_t> params,
    std::vector<uint8_t> intervals,
    uint8_t initStages,
    int32_t stepThreshold)
{
    stages.reserve(params.size());
    auto paramsBegin = params.cbegin();
    auto paramsEnd = params.cend();
    auto intervalsIt = intervals.cbegin();
    auto intervalsEnd = intervals.cend();
    if (!initStages) {
        initStages = params.size(); // if initStages is zero, init all
    }
    for (auto paramsIt = paramsBegin; paramsIt < paramsEnd; paramsIt++, intervalsIt++) {
        auto interval = (intervalsIt < intervalsEnd) && (*intervalsIt != 0) ? *intervalsIt : IirFilter::FilterDefinition(*paramsIt).downsample;
        if (paramsIt - paramsBegin < initStages) {
            stages.emplace_back(Stage{std::make_unique<IirFilter>(*paramsIt, stepThreshold), *paramsIt, interval});
        } else {
            stages.emplace_back(Stage{std::unique_ptr<IirFilter>(), *paramsIt, interval});
        }
    }
}

std::vector<FilterChain::Stage>::iterator
FilterChain::selectStage(uint8_t filterNr)
{
    auto stage = stages.begin() + filterNr;
    if (stage >= stages.end()) {
        stage = stages.end() - 1;
    }
    while (!stage->filter) {
        // if selected filter is not initialized, pick previous
        // first is always initialized
        stage--;
    }
    return stage;
}

std::vector<FilterChain::Stage>::const_iterator
FilterChain::selectStage(uint8_t filterNr) const
{
    auto stage = stages.cbegin() + filterNr;
    if (stage >= stages.cend()) {
        stage = stages.cend() - 1;
    }
    while (!stage->filter) {
        // if selected filter is not initialized, pick previous
        // first is always initialized
        stage--;
    }
    return stage;
}

FilterChain::FilterChain(
    std::vector<uint8_t> _params)
    : FilterChain(std::move(_params), std::vector<uint8_t>())
{
}

void
FilterChain::add(int32_t val)
{
    uint32_t updatePeriod = 1;
    int64_t nextFilterIn = val;
    uint8_t nextFilterInFractionBits = 0;
    for (auto& s : stages) {
        if (!s.filter) {
            break;
        }
        s.filter->add(nextFilterIn, nextFilterInFractionBits);
        updatePeriod *= s.interval; // calculate how often the next filter should be updated
        if (counter % updatePeriod != updatePeriod - 1) {
            break; // only move onto next filter if it needs to be updated
        }
        nextFilterInFractionBits = s.filter->fractionBits();
        nextFilterIn = s.filter->readWithNFractionBits(nextFilterInFractionBits);
    }
    counter++;
    if (counter == sampleInterval()) {
        counter = 0; // reset counter if last filter has had all its updates
    }
}

void
FilterChain::reset(int32_t value)
{
    for (auto& s : stages) {
        if (!s.filter) {
            break;
        }
        s.filter->reset(value);
    }
}

void
FilterChain::expandStages(size_t numStages)
{
    auto currentSize = length();
    if (numStages <= currentSize) {
        return;
    }
    auto threshold = getStepThreshold();
    auto currentOutput = read();
    for (auto it = stages.begin(); it < stages.end() && it < stages.begin() + numStages; it++) {
        if (!it->filter) {
            it->filter = std::make_unique<IirFilter>(it->param, threshold);
            it->filter->reset(currentOutput);
        }
    }
}

void
FilterChain::setStepThreshold(int32_t threshold)
{
    int32_t adjustedThreshold = threshold;
    for (auto& s : stages) {
        if (!s.filter) {
            break;
        }
        s.filter->setStepThreshold(adjustedThreshold);
    }
}

int32_t
FilterChain::getStepThreshold() const
{
    return stages.front().filter->getStepThreshold();
}

int32_t
FilterChain::read(uint8_t filterNr) const
{
    return selectStage(filterNr)->filter->read();
}

int32_t
FilterChain::readSmooth(uint8_t filterNr) const
{

    auto stage = selectStage(filterNr);
    auto updateInterval = sampleInterval(stage - 1);
    auto elapsed = counter % updateInterval;
    int64_t latest = stage->filter->read();
    int64_t previous = stage->filter->readPrevious();

    int32_t interpolated = (latest * elapsed + previous * (updateInterval - elapsed)) / updateInterval;
    return interpolated;
}

int32_t
FilterChain::read() const
{
    return read(stages.size() - 1);
}

int32_t
FilterChain::readSmooth() const
{
    return readSmooth(stages.size() - 1);
}

int64_t
FilterChain::readWithNFractionBits(uint8_t filterNr, uint8_t bits) const
{
    return selectStage(filterNr)->filter->readWithNFractionBits(bits);
}

int64_t
FilterChain::readWithNFractionBits(uint8_t bits) const
{
    return selectStage(stages.size() - 1)->filter->readWithNFractionBits(bits);
}

uint32_t
FilterChain::sampleInterval(uint8_t filterNr) const
{
    return sampleInterval(selectStage(filterNr));
}

uint32_t
FilterChain::sampleInterval(std::vector<Stage>::const_iterator stage) const
{
    uint32_t interval = 1;
    while (stage >= stages.cbegin()) {
        interval *= stage->interval;
        --stage;
    }
    return interval;
}

uint8_t
FilterChain::intervalToFilterNr(uint32_t maxInterval) const
{
    uint8_t filterNr = 0;
    uint32_t stageInterval = 1;
    auto it = stages.begin() + 1;
    while (it < stages.end()) {
        stageInterval *= it->interval;
        if (stageInterval < maxInterval) {
            filterNr++;
        } else {
            return filterNr;
        }
        ++it;
    }
    return filterNr;
}

uint32_t
FilterChain::sampleInterval() const
{
    return sampleInterval(stages.size() - 1);
}

uint8_t
FilterChain::fractionBits(uint8_t filterNr) const
{

    return selectStage(filterNr)->filter->fractionBits();
}

uint8_t
FilterChain::fractionBits() const
{
    return fractionBits(stages.size() - 1);
}

int32_t
FilterChain::readLastInput() const
{
    return stages.front().filter->readLastInput();
}

IirFilter::DerivativeResult
FilterChain::readDerivative(uint8_t filterNr) const
{
    auto stage = selectStage(filterNr);
    auto retv = stage->filter->readDerivative();
    // Scale back derivative to account for sample interval in slower updating stages
    retv.result = retv.result / sampleInterval(stage - 1);
    return retv;
}
