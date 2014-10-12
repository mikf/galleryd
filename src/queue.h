/*
 * queue.h
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

#ifndef GALLERY_QUEUE_H
#define GALLERY_QUEUE_H

#include <vector>
#include <string>
#include <regex>
#include "sqlite3pp.h"

namespace galleryd
{

class queue
{
public:
    typedef std::vector<std::string> item_t;
    typedef std::vector<int>         category_t;

    queue(const std::string &db_path);

    std::vector<int> status(item_t &items, const std::string &category);
    void             add   (item_t &items, const std::string &category);
    std::string      open  (item_t &items, const std::string &category);

    category_t match(item_t &items);

private:
    sqlite3pp::database db;
    std::vector< std::pair<std::regex, int> > re_list;
    std::size_t re_last;

    sqlite3pp::statement
    query_status_(item_t &items);

    sqlite3pp::statement
    query_status_with_category_(item_t &items, const std::string &category);

    void
    query_add_(item_t &items);

    void
    query_add_with_category_(item_t &items, const std::string &category);

    std::string
    query_open_(item_t &items);

    std::string
    query_open_with_category_(item_t &items, const std::string &category);

    int
    query_category_id_(const std::string &category);
};

}

#endif /* GALLERY_QUEUE_H */
