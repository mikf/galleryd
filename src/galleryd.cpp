/*
 * galleryd.cpp
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

#include <cstdlib>
#include "sqlite3pp.h"
#include "config.h"
#include "httpd.h"

#include <signal.h>  // sigchld handling

#define CONFIG_DIR    "gallery"
#define DATABASE_NAME "db2.sqlite3"

int main(int argc, char *argv[])
{
    // auto-reap zombie child processes
    if(signal(SIGCHLD, SIG_IGN) == SIG_ERR)
        perror(0);

    // start http deamon
    galleryd::httpd daemon {
        6411,
        get_config_dir() + "/" CONFIG_DIR "/" DATABASE_NAME
    };
    daemon.wait();

    return 0;
}
