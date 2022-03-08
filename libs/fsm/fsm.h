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

#ifndef FSM_FSM_H
#define FSM_FSM_H

#include <cassert>

#include <memory>
#include <vector>

namespace fsm {
namespace details {

template<typename TEv, typename TTbl>
typename TTbl::value_type _follow_flat(const TEv& ev, const TTbl& tbl) { return tbl[ev]; }

template<typename TEv, typename TTbl>
typename TTbl::mapped_type _follow_flex(const TEv& ev, const TTbl& tbl)
{
    typename TTbl::const_iterator it = tbl.find(ev);
    if (it != tbl.cend()) {
        return it->second;
    }
    return 0;
}

template<typename TEv, typename TSt, typename TTbl>
bool _insert_flat(const TEv& ev, const TSt& st, TTbl& tbl)
{
    if (tbl.size() > (size_t)ev) {
        tbl[ev] = st;
        return true;
    }
    return false;
}

template<typename TEv, typename TSt, typename TTbl>
bool _insert_flex(const TEv& ev, const TSt& st, TTbl& tbl) { return tbl.emplace(ev, st).second; }

} // namespace details

template<typename TEv, typename TSt, typename TTbl, bool TIsFlat>
struct trans_traits final
{
    using event_type = TEv;
    using state_type = TSt;
    using table_type = TTbl;

    static constexpr bool is_flat = TIsFlat;
};

/**
 *  \tparam TTrans
 *  \tparam TStateCont
 */
template<typename TTrans, template<typename> class TStateCont = std::vector>
class fsm
{
private:
    struct state_t final
    {
        typename TTrans::state_type follow(const typename TTrans::event_type& ev) const
        {
            if constexpr (TTrans::is_flat) {
                return details::_follow_flat(ev, table);
            } else {
                return details::_follow_flex(ev, table);
            }
        }
        
        bool insert(const typename TTrans::event_type& ev, const typename TTrans::state_type& st)
        {
            if constexpr (TTrans::is_flat) {
                return details::_insert_flat(ev, st, table);
            } else {
                return details::_insert_flex(ev, st, table);
            }
        }

        typename TTrans::table_type table;
        bool is_available = false;
    };

    using state_table = TStateCont<state_t>;

public:
    using event_type = typename TTrans::event_type;
    using ptr = std::shared_ptr<fsm<TTrans, TStateCont>>;
    using state_id = typename TTrans::state_type;

    static constexpr state_id begin_state = 1u;
    static constexpr state_id invalid_state = 0u;

    fsm()
        : m_states(2, state_t()) // invalid state 0 and begin state 1
    {}

    explicit fsm(const size_t reserve_size)
    {
        m_states.reserve(reserve_size);
        m_states.emplace_back(); // invalid state 0
        m_states.emplace_back(); // begin state 1
    }

    const state_id& begin() const { return begin_state; }

    void clear() { m_states.clear(); }

    state_id follow(const state_id& st, const event_type& ev) const
    {
        assert((st < m_states.size()) && "fsm::follow(): invalid state");
        return m_states[st].follow(ev);
    }

    template<template<typename> class TCont>
    bool follow(const TCont<event_type>& cnt) const
    {
        state_id st = begin_state;
        for (const event_type& ev : cnt) {
            st = follow(st, ev);
            if (st == invalid_state) {
                return false;
            }
        }
        return is_available(st);
    }

    template<template<typename> class TCont>
    bool insert(const TCont<event_type>& cnt)
    {
        state_id st = begin_state;
        for (size_t i = 0; i < cnt.size(); ++i) {
            const event_type& ev = cnt[i];
            state_id st_to = follow(st, ev);
            if (st_to == invalid_state) {
                st_to = insert(st, ev, false);
            }
            st = st_to;
        }
        make_available(st);
        return true;
    }

    state_id insert(const state_id& from, const event_type& ev, const bool is_available = false)
    {
        assert((from < m_states.size()) && "fsm::insert(): invalid state 'from'");

        const state_id to = make_state_id();

        if (! m_states[from].insert(ev, to)) {
            return invalid_state;
        }
        m_states[to].is_available = is_available;
        return to;
    }

    const state_id& invalid() const { return invalid_state; }

    bool is_available(const state_id& st) const
    {
        assert((st < m_states.size()) && "fsm::is_finite(): invalid state");
        return m_states[st].is_available;
    }

    void make_available(const state_id& st) { m_states[st].is_available = true; }

    state_id make_state_id()
    {
        m_states.emplace_back();
        return (m_states.size() - 1);
    }

    void reserve(const size_t size) const { m_states.reserve(size); }

    size_t size() const { return m_states.size(); }

private:
    state_table m_states;
};

} // namespace fsm

#endif // FSM_FSM_H

