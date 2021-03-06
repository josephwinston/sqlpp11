/*
 * Copyright (c) 2013-2014, Roland Bock
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 *   Redistributions of source code must retain the above copyright notice, this
 *   list of conditions and the following disclaimer.
 * 
 *   Redistributions in binary form must reproduce the above copyright notice, this
 *   list of conditions and the following disclaimer in the documentation and/or
 *   other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SQLPP_ASSIGNMENT_H
#define SQLPP_ASSIGNMENT_H

#include <sqlpp11/default_value.h>
#include <sqlpp11/null.h>
#include <sqlpp11/tvin.h>
#include <sqlpp11/serialize.h>
#include <sqlpp11/serializer.h>
#include <sqlpp11/simple_column.h>

namespace sqlpp
{
	template<typename T, typename Enable = void>
		struct is_trivial_t
		{
			static constexpr bool _(const T&)
			{
				return false;
			}
		};

	template<typename T>
		struct is_trivial_t<T, typename std::enable_if<std::is_member_function_pointer<decltype(&T::_is_trivial)>::value, void>::type>
		{
			static bool _(const T& t)
			{
				return t._is_trivial();
			}
		};

	template<typename T>
		bool is_trivial(const T& t)
		{
			return is_trivial_t<T>::_(t);
		}

	template<typename Lhs, typename Rhs>
		struct assignment_t
		{
			using _traits = make_traits<no_value_t, ::sqlpp::tag::assignment>;
			using _recursive_traits = make_recursive_traits<Lhs, Rhs>;

			using _column_t = Lhs;
			using _value_t = Rhs;

			static_assert(can_be_null_t<_column_t>::value ? true : not std::is_same<_value_t, null_t>::value, "column must not be null");

			assignment_t(_column_t lhs, _value_t rhs):
				_lhs(lhs), 
				_rhs(rhs)
			{}

			assignment_t(const assignment_t&) = default;
			assignment_t(assignment_t&&) = default;
			assignment_t& operator=(const assignment_t&) = default;
			assignment_t& operator=(assignment_t&&) = default;
			~assignment_t() = default;

			_column_t _lhs;
			_value_t _rhs;
		};

	template<typename Context, typename Lhs, typename Rhs>
		struct serializer_t<Context, assignment_t<Lhs, Rhs>>
		{
			using T = assignment_t<Lhs, Rhs>;

			static Context& _(const T& t, Context& context)
			{
				if ((trivial_value_is_null_t<typename T::_column_t>::value
							and is_trivial(t._rhs))
						or (std::is_same<Rhs, null_t>::value))
				{
					serialize(simple_column(t._lhs), context);
					context << "=NULL";
				}
				else
				{
					serialize(simple_column(t._lhs), context);
					context << "=";
					serialize(t._rhs, context);
				}
				return context;
			}
		};

	template<typename Lhs, typename Rhs>
		struct assignment_t<Lhs, tvin_t<Rhs>>
		{
			using _traits = make_traits<no_value_t, ::sqlpp::tag::assignment>;
			using _recursive_traits = make_recursive_traits<Lhs, Rhs>;

			using _column_t = Lhs;
			using _value_t = tvin_t<Rhs>;

			static_assert(can_be_null_t<_column_t>::value, "column cannot be null");

			assignment_t(_column_t lhs, _value_t rhs):
				_lhs(lhs), 
				_rhs(rhs)
			{}

			assignment_t(const assignment_t&) = default;
			assignment_t(assignment_t&&) = default;
			assignment_t& operator=(const assignment_t&) = default;
			assignment_t& operator=(assignment_t&&) = default;
			~assignment_t() = default;

			_column_t _lhs;
			_value_t _rhs;
		};

	template<typename Context, typename Lhs, typename Rhs>
		struct serializer_t<Context, assignment_t<Lhs, tvin_t<Rhs>>>
		{
			using T = assignment_t<Lhs, tvin_t<Rhs>>;

			static Context& _(const T& t, Context& context)
			{
				serialize(simple_column(t._lhs), context);
				if (t._rhs._value._is_trivial())
				{
					context << "=NULL";
				}
				else
				{
					context << "=";
					serialize(t._rhs._value, context);
				}
				return context;
			}
		};
}

#endif
