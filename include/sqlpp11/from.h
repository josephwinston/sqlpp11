/*
 * Copyright (c) 2013, Roland Bock
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

#ifndef SQLPP_FROM_H
#define SQLPP_FROM_H

#include <ostream>
#include <vector>
#include <sqlpp11/select_fwd.h>
#include <sqlpp11/type_traits.h>
#include <sqlpp11/detail/serializable.h>
#include <sqlpp11/detail/serialize_tuple.h>

namespace sqlpp
{
	template<typename... TableOrJoin>
		struct from_t
		{
			using _is_from = tag_yes;

			static_assert(sizeof...(TableOrJoin), "at least one table or join argument required in from");

			//understand joins
			//analyze tables and joins for duplicates
			//produce a set of tables in this from 

			template<typename Db>
				void serialize(std::ostream& os, Db& db) const
				{
					os << " FROM ";
					detail::serialize_tuple(os, db, _tables, ',');
				}

			std::tuple<TableOrJoin...> _tables;
		};

	template<typename Db, typename... TableOrJoin>
	struct dynamic_from_t
	{
		using _is_from = tag_yes;
		using _is_dynamic_from = tag_yes;

		template<typename Table>
		void add(Table&& table)
		{
			_dynamic_tables.push_back(std::forward<Table>(table));
		}

		void serialize(std::ostream& os, Db& db, bool has_static_from) const
		{
			if (sizeof...(TableOrJoin) == 0 and _dynamic_tables.empty())
				return;
			os << " FROM ";
			detail::serialize_tuple(os, db, _tables, ',');
			bool first = sizeof...(TableOrJoin) == 0;
			for (const auto& table : _dynamic_tables)
			{
				if (not first)
					os << ',';
				table.serialize(os, db);
				first = false;
			}
		}

		std::tuple<TableOrJoin...> _tables;
		std::vector<detail::serializable_t<Db>> _dynamic_tables;
	};
}

#endif