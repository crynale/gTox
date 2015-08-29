/**
    gTox a GTK-based tox-client - https://github.com/KoKuToru/gTox.git

    Copyright (C) 2015  Luca Béla Palkovics

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>
**/
#ifndef GTOX_DEBUG_H
#define GTOX_DEBUG_H

#ifdef __GNUG__
    #include <cstdlib>
    #include <memory>
    #include <cxxabi.h>
#endif

#include <typeinfo>
#include <typeindex>
#include <map>
#include <iostream>

namespace utils {
    struct debug_helper_struct {
            std::string name;
            int count;
    };

    extern std::map<std::type_index, debug_helper_struct> debug_helper_counter;

    template<typename T>
    class debug {
        private:
            std::string demangle(const char* name) {
#ifdef __GNUG__
                int status = -4; // some arbitrary value to eliminate the compiler warning

                // enable c++11 by passing the flag -std=c++11 to g++
                std::unique_ptr<char, void(*)(void*)> res {
                    abi::__cxa_demangle(name, NULL, NULL, &status),
                            std::free
                };

                return (status==0) ? res.get() : name ;
#else
                return name;
#endif
            }
        public:
            debug() {
                auto& item = debug_helper_counter[std::type_index(typeid(T))];
                item.name = demangle(typeid(T).name());
                item.count += 1;
                std::clog << "+ " << (void*)this << " " << item.name << std::endl;
            }
            virtual ~debug() {
                auto& item = debug_helper_counter[std::type_index(typeid(T))];
                std::clog << "- " << (void*)this << " " << item.name << std::endl;
                item.count -= 1;
            }

            auto& log() {
                return std::clog << "  " << (void*)this << " ";
            }
    };

    /**
     * @brief The log class, prints a log message on creation and destruction
     */
    class log {
        public:
            log();
            ~log();
    };
}
#endif
