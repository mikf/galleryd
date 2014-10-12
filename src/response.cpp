/*
 * response.cpp
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

#include "response.h"

#include <exception>
#include <cstring>    // strlen

#include <unistd.h>   // fork, execl
#include <sys/stat.h>
#include <fcntl.h>    // dup2. open

galleryd::response::response()
    : handle_(yajl_gen_alloc(nullptr))
{
        // TODO error handling
}

galleryd::response::~response()
{
    if(handle_ != nullptr)
        yajl_gen_free(handle_);
}

void galleryd::response::buffer(const char **buf, size_t *len) const
{
    yajl_gen_get_buf(handle_, reinterpret_cast<const unsigned char**>(buf), len);
}

void galleryd::response::generate(galleryd::request &req, galleryd::queue &q)
{
    yajl_gen_clear(handle_);
    yajl_gen_map_open(handle_);

    try
    {

    if(req.method == "status")
    {
        constexpr const unsigned char key_states[] = {'s', 't', 'a', 't', 'e'};

        yajl_gen_string(handle_, key_states, sizeof(key_states));
        yajl_gen_array_open(handle_);
        for(auto state : q.status(req.items, req.category))
            yajl_gen_integer(handle_, state);
        yajl_gen_array_close(handle_);
    }

    else if(req.method == "add")
    {
        q.add(req.items, req.category);
    }

    else if(req.method == "open")
    {
        auto path = q.open(req.items, req.category);
        if(not path.empty())
        {
            path += '/';
            path += req.items[0];

            if(fork() == 0)
            {
                int fd = open("/dev/null", O_WRONLY);
                dup2(fd, 2);  // redirect stderr
                execl("/usr/bin/geeqie", "/usr/bin/geeqie", "-r", path.c_str(), nullptr);
                exit(1); // in case execl returns with an error
            }
        }
        else
        {
            // TODO error handling
        }
    }

    }
    catch(const std::exception &e)
    {
        constexpr const unsigned char key_error[] = {'e', 'r', 'r', 'o', 'r'};
        const char  *err = e.what();
        const size_t len = strlen(err);

        yajl_gen_string(handle_, key_error, sizeof(key_error));
        yajl_gen_string(handle_, reinterpret_cast<const unsigned char *>(err), len);
    }

    yajl_gen_map_close(handle_);
}
