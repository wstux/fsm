/*
 * The MIT License
 *
 * Copyright 2022 Chistyakov Alexander.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef FSM_TRIE_H
#define FSM_TRIE_H

#include <map>
#include <vector>

#include "fsm/fsm.h"

namespace fsm {

template<typename TValue, typename TTrans, template<typename> class TStateCont = std::vector,
         typename TValueCont = std::map<typename TTrans::state_type, TValue>>
class trie
{
    using fsm_type = fsm<TTrans, TStateCont>;
    using trie_type = trie<TValue, TTrans, TStateCont, TValueCont>;
    using value_list = TValueCont;

public:
    using event_type = typename fsm_type::event_type;
    using ptr = std::shared_ptr<trie_type>;
    using state_id = typename fsm_type::state_id;
    using pointer_type = const TValue* const;
    using value_type = TValue;

    trie()
        : m_fsm()
    {}

    explicit trie(const size_t reserve_size)
        : m_fsm(reserve_size)
    {}

    trie(const trie& other)
        : m_fsm(other.m_fsm)
        , m_values(other.m_values)
    {}

    trie(trie&& other)
        : m_fsm(std::move(other.m_fsm))
        , m_values(std::move(other.m_values))
    {}

    trie& operator=(const trie& other)
    {
        if (this == &other) {
            return *this;
        }
        m_fsm = other.m_fsm;
        m_values = other.m_values;
        return *this;
    }

    trie& operator=(trie&& other)
    {
        if (this == &other) {
            return *this;
        }
        m_fsm = std::move(other.m_fsm);
        std::swap(m_values, other.m_values);
        return *this;
    }

    const state_id& begin() const { return m_fsm.begin(); }

    void clear() { m_fsm.clear(); }

    state_id follow(state_id& st, const event_type& ev) const { return m_fsm.follow(st, ev); }

    bool follow(state_id& st, const event_type& ev, value_type& val) const
    {
        st = m_fsm.follow(st, ev);
        if (st == invalid()) {
            return false;
        }
        val = value(st);
        return true;
    }

    template<template<typename> class TCont>
    bool follow(const TCont<event_type>& cnt) const { return m_fsm.follow(cnt); }

    template<template<typename> class TCont>
    bool follow(const TCont<event_type>& cnt, value_type& val) const
    {
        state_id st = begin();
        for (const event_type& ev : cnt) {
            st = follow(st, ev);
            if (st == invalid()) {
                return false;
            }
        }
        if (is_available(st)) {
            val = value(st);
            return true;
        }
        return false;
    }

    template<template<typename> class TCont>
    bool insert(const TCont<event_type>& cnt, const value_type& val)
    {
        state_id st = begin();
        for (size_t i = 0; i < cnt.size(); ++i) {
            const event_type& ev = cnt[i];
            state_id st_to = follow(st, ev);
            if (st_to == invalid()) {
                st_to = insert(st, ev, false);
            }
            st = st_to;
        }
        m_fsm.make_available(st);
        m_values[st] = val;
        return true;
    }

    state_id insert(const state_id& from, const event_type& ev, const bool is_available = false)
    {
        return m_fsm.insert(from, ev, is_available);
    }

    const state_id& invalid() const { return m_fsm.invalid(); }

    bool is_available(const state_id& st) const { return m_fsm.is_available(st); }

    void reserve(const size_t size) const { m_fsm.reserve(size); }

    void set_value(const state_id& st, const value_type& val) { m_values[st] = val; }

    size_t size() const { return m_fsm.size(); }

    void swap(trie& other)
    {
        if (this == &other) {
            return;
        }
        m_fsm.swap(other.m_fsm);
        std::swap(m_values, other.m_values);
    }

    const value_type& value(const state_id& st) const { return m_values.at(st); }

private:
    fsm_type m_fsm;
    value_list m_values;
};

} // namespace fsm

#endif // FSM_TRIE_H

