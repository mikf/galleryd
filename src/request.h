/*
 * request.h
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

#ifndef GALLERY_REQUEST_H
#define GALLERY_REQUEST_H

#include <yajl/yajl_parse.h>
#include <string>
#include <vector>

namespace galleryd
{

class request
{
public:

    request();
    ~request();

    void parse(const char *data, size_t len);
    void complete();

    std::string method;
    std::string category;
    std::vector<std::string> items;

private:
    enum parse_state
    {
        STATE_METHOD,
        STATE_CATEGORY,
        STATE_ITEMS,
    };

    yajl_handle handle;
    parse_state state;

    static const yajl_callbacks callbacks;
    static int handle_string (void *ctx, const unsigned char *data, size_t len);
    static int handle_map_key(void *ctx, const unsigned char *key,  size_t len);
};

}

#endif /* GALLERY_REQUEST_H */
