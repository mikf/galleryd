/*
 * config.h
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

#ifndef CONFIG_H
#define CONFIG_H

#include <map>
#include <string>
#include <fstream>

////////////////////////////////////////////////////////////////////////////////
// class config
//
// A simple extension to the std::map container, that allows reading and writing
// of key-value pairs from ini-style configuration files
class config
    : public std::map< std::string, std::map<std::string, std::string> >
{
public:
    template <typename... Args>
    explicit config(Args&&... args)
    {
        readall(std::forward<Args>(args)...);
    }

    // loading key-value pairs from an ini-like file or stream
    void read(const char * filename);
    void read(const std::string & filename);
    void read(std::istream & stream);

    // variadic template to load values from multiple sources
    // for example:
    //      readall("config1.ini", filestream, "/etc/config.ini");
    // is equivalent to
    //      read("config1.ini");
    //      read(filestream);
    //      read("/etc/config.ini");
    template <typename T, typename... Args>
    inline void readall(T&& arg, Args&&... args)
    {
        read(std::forward<T>(arg));
        readall(std::forward<Args>(args)...);
    }
    inline void readall() const {}

    // writing key-value pairs in ini-file format
    void write(const char * filename);
    void write(const std::string & filename);
    void write(std::ostream & stream);
};

////////////////////////////////////////////////////////////////////////////////
// get_config_dir
std::string get_config_dir();

#endif /* CONFIG_H */
