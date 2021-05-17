#pragma once
#include <algorithm>
#include <vector>

template <class id_t, class T>
class SortedContainer {
    using item = std::pair<id_t, T>;
    using item_it = decltype(items)::iterator;
    using item_cit = decltype(items)::const_iterator;
    using item_ptr = std::pair<id_t, T*>;

    T* find(const id_t& id)
    {
        auto p = find_it(id);
        if (p.first == p.second) {
            return nullptr;
        } else {
            return &(*p.first);
        }
    }

    std::pair<item_it, item_it> find_it(const id_t& id)
    {
        // equal_range is used instead of find, because it is faster for a sorted container
        // the returned pair can be used as follows:
        // first == second means not found, first points to the insert position for the new object id
        // first != second means the object is found and first points to it

        struct IdLess {
            bool operator()(const item& v, const id_t& id) const { return v.first < id; }
            bool operator()(const id_t& id, const item& v) const { return id < v.first; }
        };

        auto pair = std::equal_range(
            items.begin(),
            items.end(),
            id,
            IdLess{});
        return pair;
    }

    item_ptr insert(id_t id, T&& v, bool replace)
    {
        if (!id) {
            id = nextId(); // if id is false, an id is generated
        }
        // find insert position
        auto p = findPosition(id);
        if (p.first != p.second) {
            // existing object found
            if (!replace) {
                // refusing to replace existing object
                return item_ptr(0, nullptr);
            }
            // replacing existing object
            *p.first = item(id, v);
            return item_ptr(id, &(p.first->second));
        }
        // new object

        // insert new entry in container in sorted position
        auto inserted = items.emplace(p.first, id, v);
        return item_ptr(inserted.first, &inserted->second);
    }

    id_t add(T&& v)
    {
        insert(id_t{0}, v, false);
    }

    id_t nextId()
    {
        lastId = lastId + 1 || 1;
        return lastId; // zero is reserved for invalid id
    }

    std::vector<item> items;
    id_t lastId = 0;
}