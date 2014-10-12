/*
 * httpd.cpp
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

#include "httpd.h"
#include "request.h"
#include "response.h"

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>
#include <netinet/in.h>



struct httpd_data
{
    galleryd::request  req;
    galleryd::response res;
};



galleryd::httpd::httpd(unsigned short port, const std::string &db_path)
    : queue_(db_path)
    , handle_(nullptr)
    , fd_()
{
    if(pipe(fd_) != 0)
        throw "nope";

    handle_ = MHD_start_daemon(
        MHD_USE_SELECT_INTERNALLY | MHD_USE_DEBUG, port,
        handle_accept , this,
        handle_request, this,
        MHD_OPTION_NOTIFY_COMPLETED, handle_complete, this,
        MHD_OPTION_END);

    if(handle_ == nullptr)
    {
        close(fd_[0]);
        close(fd_[1]);
        throw "nope";
    }
}

galleryd::httpd::~httpd()
{
    if(handle_ != nullptr)
        MHD_stop_daemon(handle_);
    close(fd_[0]);
    close(fd_[1]);
}

void galleryd::httpd::wait()
{
    char buf[4];

    while(read(fd_[0], buf, sizeof(buf)) == -1);
}

void galleryd::httpd::quit()
{
    MHD_stop_daemon(handle_);
    write(fd_[1], "EXT", 4);
}



int galleryd::httpd::handle_accept(
    void *cls,
    const sockaddr * addr,
    socklen_t addrlen)
{
    constexpr const uint32_t localhost = 16777343;

    if(addr->sa_family == AF_INET &&
        ((const sockaddr_in*)addr)->sin_addr.s_addr == localhost)
    {
        return MHD_YES;
    }

    return MHD_NO;
}

int galleryd::httpd::handle_request(
    void *cls,
    MHD_Connection *connection,
    const char *url,
    const char *method,
    const char *version,
    const char *upload_data,
    size_t *upload_data_size,
    void **con_cls)
{
    if(*con_cls == nullptr) /* first call for this connection */
    {
        if(strcmp("POST", method) != 0)
            return MHD_NO; // reject requests other than POST

        // allocate connection-specific data
        *con_cls = new httpd_data;

        return MHD_YES;
    }

    httpd      * d = static_cast<httpd *>(cls);
    httpd_data *hd = static_cast<httpd_data *>(*con_cls);

    if(*upload_data_size) /* handle post data */
    {
        hd->req.parse(upload_data, *upload_data_size);
        *upload_data_size = 0;
        return MHD_YES;
    }

    else /* generate response */
    {
        const char *buf;
        size_t len;

        hd->req.complete();
        hd->res.generate(hd->req, d->queue_);
        hd->res.buffer(&buf, &len);

        MHD_Response *response = MHD_create_response_from_buffer(
            len, const_cast<char *>(buf), MHD_RESPMEM_PERSISTENT);
        int ret = MHD_queue_response(connection, MHD_HTTP_OK, response);
        MHD_destroy_response(response);

        return ret;
    }
}

void galleryd::httpd::handle_complete(
    void *cls,
    MHD_Connection *connection,
    void **con_cls,
    MHD_RequestTerminationCode toe)
{
    httpd      * d = static_cast<httpd *>(cls);
    httpd_data *hd = static_cast<httpd_data *>(*con_cls);

    if(hd == nullptr)
        return;

    if(hd->req.method == "quit")
        d->quit();

    delete hd;
    *con_cls = nullptr;
}
