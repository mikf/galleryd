/*
 * request.cpp
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

#include "request.h"
#include <cstring>

const yajl_callbacks galleryd::request::callbacks = {
    nullptr, // null
    nullptr, // bool
    nullptr, // integer
    nullptr, // double
    nullptr, // number
    galleryd::request::handle_string,

    nullptr, // map start
    galleryd::request::handle_map_key,
    nullptr, // map end

    nullptr, // array start
    nullptr, // array end
};

galleryd::request::request()
    : handle(yajl_alloc(&callbacks, nullptr, this))
{
}

galleryd::request::~request()
{
    if(handle)
        yajl_free(handle);
}

void galleryd::request::parse(const char *data, size_t len)
{
    yajl_parse(handle, reinterpret_cast<const unsigned char*>(data), len);
}

void galleryd::request::complete()
{
    yajl_complete_parse(handle);
}

int galleryd::request::handle_string(void *ctx, const unsigned char *data, size_t len)
{
    request *self = static_cast<request *>(ctx);
    const char * d = reinterpret_cast<const char *>(data);

    switch(self->state)
    {
    case STATE_METHOD:
        self->method.assign(d, len);
        break;
    case STATE_CATEGORY:
        self->category.assign(d, len);
        break;
    case STATE_ITEMS:
        self->items.emplace_back(d, len);
        break;
    }
    return true;
}

int galleryd::request::handle_map_key(void *ctx, const unsigned char *key, size_t len)
{
    request *self = static_cast<request *>(ctx);
    const char * k = reinterpret_cast<const char *>(key);

         if(strncmp("method"  , k, len) == 0)
        self->state = STATE_METHOD;
    else if(strncmp("category", k, len) == 0)
        self->state = STATE_CATEGORY;
    else if(strncmp("items"   , k, len) == 0)
        self->state = STATE_ITEMS;
    return true;
}
