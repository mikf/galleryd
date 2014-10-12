/*
 * response.h
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

#ifndef GALLERY_RESPONSE_H
#define GALLERY_RESPONSE_H

#include <yajl/yajl_gen.h>
#include "queue.h"
#include "request.h"

namespace galleryd
{

class response
{
public:
    response();
    ~response();

    void generate(galleryd::request &req, galleryd::queue &q);
    void buffer(const char **buf, size_t *len) const;

private:
    yajl_gen handle_;
};

}

#endif /* GALLERY_RESPONSE_H */
