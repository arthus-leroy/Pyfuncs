/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 * Copyright (c) 2019, Arthus Leroy <arthus.leroy@epita.fr>
 * On https://github.com/arthus-leroy/Pyfuncs
 * All rights reserved. */

# pragma once

# include <vector>
# include <assert.h>

# include "misc.hh"

/** range
 *  Equivalent of python's range
 *  Called as :
 *      - for (const auto i : range(end)) with end any size_t
 *      - for (const auto i : range(begin, end)) with begin, end any size_t
 */
class range
{
    class Iterator : std::iterator<std::input_iterator_tag, std::size_t, void, std::size_t*, std::size_t&>
    {
    public:
        Iterator(std::size_t max)
            : i_(0), max_(max)
        {}

        Iterator(std::size_t i, std::size_t max)
            : i_(i), max_(max)
        {}

        bool operator!=(const Iterator& iter)
        {
            return i_ != iter.i_;
        }

        std::size_t operator*()
        {
            return i_;
        }

        Iterator& operator++()
        {
            i_++;
            return *this;
        }

    private:
        std::size_t i_;
        const std::size_t max_;
    };

public:
    range(const std::size_t i, const std::size_t max)
        : i_(i), max_(max)
    {
        assert(i <= max);
    }

    range(const std::size_t max)
        : range(0, max)
    {}

    Iterator begin()
    {
        return Iterator(i_, max_);
    }

    Iterator end()
    {
        return Iterator(max_, max_);
    }

private:
    const std::size_t i_;
    const std::size_t max_;
};

/** zip
 *  Equivalent of python's zip
 *  Called as :
 *      for (const auto& [a, b, ...] : zip(v0, v1, ...)) with v0, v1, ... any random access container
 */
template <typename ...Args>
class zip
{
    using array_t = std::tuple<const Args&...>;
    using tuple_t = std::tuple<typename Args::value_type...>;

    template <std::size_t N>
    class Iterator : std::iterator<std::input_iterator_tag, tuple_t, void, tuple_t*, tuple_t&>
    {
        template <std::size_t M>
        auto get()
        {
            static_assert(M < N);

            if constexpr(M == N - 1)    // size - 1
                return std::make_tuple(std::get<M>(arrays_)[i_]);
            else
                return std::tuple_cat(std::make_tuple(std::get<M>(arrays_)[i_]), get<M + 1>());
        }

    public:
        Iterator(const array_t& arrays)
            : arrays_(arrays), i_(0)
        {}

        Iterator(const array_t& arrays, const std::size_t i)
            : arrays_(arrays), i_(i)
        {}

        bool operator!=(const Iterator& iter)
        {
            return i_ != iter.i_ || &arrays_ != &iter.arrays_;
        }

        tuple_t operator*()
        {
            return get<0>();
        }

        Iterator& operator++()
        {
            i_++;
            return *this;
        }

    private:
        const array_t& arrays_;
        std::size_t i_;
    };

public:
    zip(const Args&... args)
        : arrays_(args...)
        , max_(min_size(args...))
    {}

    auto begin()
    {
        return Iterator<sizeof...(Args)>(arrays_, 0);
    }

    auto end()
    {
        return Iterator<sizeof...(Args)>(arrays_, max_);
    }

private:
    const array_t arrays_;
    const std::size_t max_;
};