/*
 * Copyright (c) 2013-2014, Roland Bock
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification, 
 * are permitted provided that the following conditions are met:
 * 
 *  * Redistributions of source code must retain the above copyright notice, 
 *    this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above copyright notice, 
 *    this list of conditions and the following disclaimer in the documentation 
 *    and/or other materials provided with the distribution.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND 
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED 
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. 
 * IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, 
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, 
 * BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, 
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE 
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED 
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef SQLPP_MOCK_DB_H
#define SQLPP_MOCK_DB_H

#include <sstream>
#include <sqlpp11/serializer_context.h>
#include <sqlpp11/connection.h>

struct MockDb: public sqlpp::connection
{
	struct _serializer_context_t
	{
		std::ostringstream _os;

		std::string str() const
		{
			return _os.str();
		}

		void reset()
		{
			_os.str("");
		}

		template<typename T>
			std::ostream& operator<<(T t)
			{
				return _os << t;
			}

		static std::string escape(std::string arg)
		{
			return sqlpp::serializer_context_t::escape(arg);
		}
	};

	using _interpreter_context_t = _serializer_context_t;

	template<typename T>
	static _serializer_context_t& _serialize_interpretable(const T& t, _serializer_context_t& context)
	{
		sqlpp::serialize(t, context);
		return context;
	}

	template<typename T>
	static _serializer_context_t& _interpret_interpretable(const T& t, _interpreter_context_t& context)
	{
		sqlpp::serialize(t, context);
		return context;
	}

	class result_t
	{
	public:
		constexpr bool operator==(const result_t& rhs) const
		{
			return true;
		}

		template<typename ResultRow>
			void next(ResultRow& result_row)
			{
				result_row._invalidate();
			};
	};

	// Directly executed statements start here
	template<typename T>
		auto operator() (const T& t) -> decltype(t._run(*this))
		{
			return t._run(*this);
		}

	template<typename Insert>
		size_t insert(const Insert& x)
		{
			return 0;
		}

	template<typename Update>
		size_t update(const Update& x)
		{
			return 0;
		}

	template<typename Remove>
		size_t remove(const Remove& x)
		{
			return 0;
		}

	template<typename Select>
		result_t select(const Select& s)
		{
			return {};
		}

	// Prepared statements start here
	using _prepared_statement_t = std::nullptr_t;

	template<typename T>
		auto prepare(const T& t) -> decltype(t._prepare(*this))
		{
			return t._prepare(*this);
		}


	template<typename Insert>
		_prepared_statement_t prepare_insert(Insert& x)
		{
			return nullptr;
		}

	template<typename PreparedInsert>
		size_t run_prepared_insert(const PreparedInsert& x)
		{
			return 0;
		}

	template<typename Select>
		_prepared_statement_t prepare_select(Select& x)
		{
			return nullptr;
		}

	template<typename PreparedSelect>
		result_t run_prepared_select(PreparedSelect& x)
		{
			return {};
		}

};

#endif

