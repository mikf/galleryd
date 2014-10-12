/*
 * httpd.h
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

#ifndef GALLERY_HTTPD_H
#define GALLERY_HTTPD_H

#include <sys/types.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <microhttpd.h>
#include "sqlite3pp.h"
#include "queue.h"

namespace galleryd
{

class httpd
{
public:

    httpd(unsigned short port, const std::string &db_path);
    ~httpd();

    void wait();
    void quit();

private:
    galleryd::queue queue_;
    MHD_Daemon *handle_;
    int fd_[2];

    static int handle_accept(
        void *cls,
        const sockaddr * addr,
        socklen_t addrlen);

    static int handle_request(
        void *cls,
        MHD_Connection *connection,
        const char *url,
        const char *method,
        const char *version,
        const char *upload_data,
        size_t *upload_data_size,
        void **con_cls);

    static void handle_complete(
        void *cls,
        MHD_Connection *connection,
        void **con_cls,
        MHD_RequestTerminationCode toe);
};

}

#endif /* GALLERY_HTTPD_H */
