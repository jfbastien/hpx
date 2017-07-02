//  Copyright (c) 2017 Agustin Berge
//
//  Distributed under the Boost Software License, Version 1.0. (See accompanying
//  file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)

#ifndef HPX_UTIL_IOTA_RANGE_HPP
#define HPX_UTIL_IOTA_RANGE_HPP

#include <hpx/config.hpp>
#include <hpx/util/iterator_facade.hpp>

#include <cstddef>
#include <iterator>
#include <utility>

namespace hpx { namespace util
{
    namespace detail
    {
        template <typename T>
        class iota_iterator
          : public util::iterator_facade<
                iota_iterator<T>, T,
                std::random_access_iterator_tag,
                T const
            >
        {
        public:
            explicit iota_iterator(T const& value)
              : _value(value)
            {}

        private:
            friend class util::iterator_core_access;

            bool equal(iota_iterator const& other) const
            {
                return _value == other._value;
            }

            T const dereference() const
            {
                return _value;
            }

            void increment()
            {
                ++_value;
            }

            void decrement()
            {
                --_value;
            }

            void advance(std::ptrdiff_t n)
            {
                _value += n;
            }

            std::ptrdiff_t distance_to(iota_iterator const& other) const
            {
                return other._value - _value;
            }

        private:
            T _value;
        };
    }

    template <typename T>
    class iota_range
    {
    public:
        iota_range(T from, T to)
          : _from(std::move(from)), _to(std::move(to))
        {}

        detail::iota_iterator<T> begin() const
        {
            return detail::iota_iterator<T>(_from);
        }

        detail::iota_iterator<T> end() const
        {
            return detail::iota_iterator<T>(_to);
        }

        std::ptrdiff_t size() const
        {
            return _to - _from;
        }

        bool empty() const
        {
            return _from == _to;
        }

    private:
        T _from, _to;
    };

    template <typename T>
    iota_range<T> make_iota_range(T from, T to)
    {
        return iota_range<T>(std::move(from), std::move(to));
    }
}}

#endif /*HPX_UTIL_IOTA_RANGE_HPP*/
