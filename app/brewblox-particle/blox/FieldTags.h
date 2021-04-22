#pragma once

#include "cbox/DataStream.h"
#include "nanopb_callbacks.h"
#include <vector>

class FieldTags {
private:
    std::vector<uint16_t> m_tags;

public:
    FieldTags() {}

    ~FieldTags() = default;

    void add(uint16_t&& t)
    {
        m_tags.push_back(t);
    }

    void copyToMessage(uint16_t* dest, pb_size_t& count, const pb_size_t& maxCount)
    {
        count = 0;
        for (auto it = m_tags.cbegin(); it != m_tags.cend() && it < m_tags.cbegin() + maxCount; ++it) {
            *(dest++) = *it;
            ++count;
        }
    }

    const std::vector<uint16_t>& tags()
    {
        return m_tags;
    }
};
