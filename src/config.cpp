/*
 * config.cpp
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

#include "config.h"
#include <cctype> // isspace

#include <sys/types.h>
#include <pwd.h>    // getpwuid
#include <unistd.h> // getuid

void config::read(const char * filename)
{
    std::ifstream stream {filename};
    read(stream);
}

void config::read(const std::string & filename)
{
    std::ifstream stream {filename};
    read(stream);
}

void config::read(std::istream & stream)
{
    std::string buffer;
    std::map<std::string, std::string> * category;

    while(std::getline(stream, buffer))
    {
        // ignore empty lines and lines starting with an "#" (comments)
        if(buffer.empty() or buffer[0] == '#')
            continue;

        if(buffer[0] == '[') // set new category
        {
            auto pos = buffer.rfind(']');
            if(pos == std::string::npos)
                continue; // ignore invalid category lines
            auto p = emplace(
                std::piecewise_construct,
                std::forward_as_tuple(buffer, 1, pos-1),
                std::forward_as_tuple());
            category = &(p.first->second);
        }
        else // 'key = value' pair
        {
            // search seperating "="
            auto vpos = buffer.find('=');
            if(vpos == std::string::npos)
                continue; // ignore invalid input
            auto kpos = vpos;

            // remove whitespace
            while(std::isspace(buffer[--kpos]));
            while(std::isspace(buffer[++vpos]));

            // insert key-value pair into config
            // auto p = category->emplace(
            category->emplace(
                std::piecewise_construct,
                std::forward_as_tuple(buffer, 0, kpos+1),
                std::forward_as_tuple(buffer, vpos));
            // if(not p.second)
                // p.first->assign(buffer, vpos, std::string::npos);
        }
    }
}

void config::write(const char * filename)
{
    std::ofstream stream {filename};
    write(stream);
}

void config::write(const std::string & filename)
{
    std::ofstream stream {filename};
    write(stream);
}

void config::write(std::ostream & stream)
{
    size_type cat = 0;
    for(auto&& category : *this)
    {
        stream << '[' << category.first << ']' << std::endl;
        for(auto&& kv : category.second)
            stream << kv.first << " = " << kv.second << std::endl;
        if(++cat == 1)
            stream << std::endl;
    }
}

std::string get_config_dir()
{
    std::string path;
    const char * value;

    value = std::getenv("XDG_CONFIG_HOME");
    if(value != nullptr && value[0] != '\0')
    {
        path.assign(value);
    }
    else
    {
        value = std::getenv("HOME");
        if(value != nullptr && value[0] != '\0')
        {
            path.assign(value);
            path.append("/.config");
        }
        else
        {
            struct passwd *pw = getpwuid(getuid());
            if(pw != nullptr)
            {
                path.assign(pw->pw_dir);
                path.append("/.config");
            }
            else
            {
                throw std::runtime_error("Could not determine configuration directory");
            }
        }
    }

    return std::move(path);
}
