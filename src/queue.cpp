/*
 * queue.cpp
 *
 * Copyright 2014 Mike Fährmann <mike_faehrmann@web.de>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "queue.h"
#include <map>

galleryd::queue::queue(const std::string &db_path)
    : db(db_path)
    , re_list()
    , re_last(0)
{
    for(auto&& row : db.prepare("SELECT re, category_id FROM regex"))
        re_list.emplace_back(std::regex(row.as_string(0)), row.as_int(1));
}

std::vector<int>
galleryd::queue::status(item_t &items, const std::string &category)
{
    std::vector<int> states (items.size());

    // build sql statement
    auto stmt = category.empty()
        ? query_status_(items)
        : query_status_with_category_(items, category);

    // insert all found items into a map
    std::map<std::string, int> lookup;
    for(auto&& row : stmt)
        lookup.emplace(row.as_string(0), row.as_int(1));

    // build array with state-values for each item in the request.
    // if the item doesn't exist, the value is '0'.
    std::map<std::string, int>::const_iterator iter, end = lookup.end();
    for(size_t i = 0, e = items.size(); i < e; ++i)
    {
        iter      = lookup.find(items[i]);
        states[i] = iter == end ? 0 : iter->second;
    }

    return states;
}

void
galleryd::queue::add(item_t &items, const std::string &category)
{
    if(category.empty())
        query_add_(items);
    else
        query_add_with_category_(items, category);
}

std::string
galleryd::queue::open(item_t &items, const std::string &category)
{
    return category.empty()
        ? query_open_(items)
        : query_open_with_category_(items, category);
}

////////////////////////////////////////////////////////////////////////////////
sqlite3pp::statement
galleryd::queue::query_status_(item_t &items)
{
    // get category-ids for items
    const auto len = items.size();
    const auto categories = match(items);

    // build query string
    std::string q {
        "SELECT item, state "
        "FROM   queue "
        "WHERE (item=? AND category_id=?)"
    };
    for(size_t i = 1; i < len; ++i)
        q += " OR (item=? AND category_id=?)";

    // bind values to statement
    auto stmt = db.prepare(q);
    for(size_t i = 0; i < len; ++i)
    {
        stmt.bind(2*i+1, items[i]);
        stmt.bind(2*i+2, categories[i]);
    }

    return stmt;
}

sqlite3pp::statement
galleryd::queue::query_status_with_category_(
    item_t            &items,
    const std::string &category)
{
    // build query string
    std::string q {
        "SELECT item, state "
        "FROM   queue JOIN category "
        "ON     queue.category_id = category.id "
        "WHERE  category.name=? AND item IN (?"
    };

    for(size_t i = 1, e = items.size(); i < e; ++i)
        q += ",?";
    q += ')';

    // bind values to statement
    int  idx  = 1;
    auto stmt = db.prepare(q);

    stmt.bind(1, category);
    for(auto&& item : items)
        stmt.bind(++idx, item);

    return stmt;
}

////////////////////////////////////////////////////////////////////////////////
void
galleryd::queue::query_add_(
    item_t &items)
{
    // get category-ids
    const auto categories = match(items);

    // build query
    auto stmt = db.prepare(
        "INSERT INTO queue "
        "(item, category_id) "
        "VALUES (?, ?)"
    );

    // add items
    auto tr = db.begin_transaction();
    for(size_t i = 0, len = items.size(); i < len; ++i)
    {
        stmt.bind(1, items[i]);
        stmt.bind(2, categories[i]);
        stmt.exec();
    }
    tr.commit();
}

void
galleryd::queue::query_add_with_category_(
    item_t            &items,
    const std::string &category)
{
    // get id from name
    const auto category_id = query_category_id_(category);

    // build query
    auto stmt = db.prepare(
        "INSERT INTO queue "
        "(item, category_id) "
        "VALUES (?, ?)"
    );
    stmt.bind(2, category_id);

    // add items
    auto tr = db.begin_transaction();
    for(size_t i = 0, len = items.size(); i < len; ++i)
    {
        stmt.bind(1, items[i]);
        stmt.exec();
    }
    tr.commit();
}

////////////////////////////////////////////////////////////////////////////////
std::string
galleryd::queue::query_open_(
    item_t &items)
{
    const auto categories = match(items);

    // build query string
    auto stmt = db.prepare(
        "SELECT path "
        "FROM   category "
        "WHERE  id=?"
    );

    // bind values to statement
    stmt.bind(1, categories[0]);

    // get  path
    auto row = stmt.begin();
    return row != stmt.end() ? row[0] : "";
}

std::string
galleryd::queue::query_open_with_category_(
    item_t            &items,
    const std::string &category)
{
    // build query string
    auto stmt = db.prepare(
        "SELECT path "
        "FROM   category "
        "WHERE  name=?"
    );

    // bind values to statement
    stmt.bind(1, category);

    // get  path
    auto row = stmt.begin();
    return row != stmt.end() ? row[0] : "";
}

////////////////////////////////////////////////////////////////////////////////
galleryd::queue::category_t
galleryd::queue::match(
    item_t &items)
{
    std::smatch match;
    const auto len = items.size();
    category_t categories (len);

    for(size_t i = 0; i < len; ++i)
    {
        // test with the last successfull regex first
        if(std::regex_match(items[i], match, re_list[re_last].first))
        {
            items[i] = match[1];
            categories[i] = re_list[re_last].second;
            continue;
        }

        for(size_t j = 0; j < re_list.size(); ++j)
        {
            if(j == re_last)
                continue;

            if(std::regex_match(items[i], match, re_list[j].first))
            {
                items[i] = match[1];
                categories[i] = re_list[j].second;
                re_last = j;
            }
        }
    }

    return categories;
}

int
galleryd::queue::query_category_id_(
    const std::string &category)
{
    auto stmt = db.prepare("SELECT id FROM category WHERE name=?");
    stmt.bind(1, category);

    auto row = stmt.begin();
    return row != stmt.end() ? row.as_int(0) : 0;
}
