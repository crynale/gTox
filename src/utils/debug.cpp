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
#include "debug.h"

std::map<std::type_index, utils::debug_helper_struct> utils::debug_helper_counter __attribute__ ((init_priority (2000)));

void display() {
    bool okay = true;
    for (auto it: utils::debug_helper_counter) {
        auto& item = it.second;
        if (item.count != 0) {
            okay = false;
            std::clog << item.name << " " << item.count << " active objects !" << std::endl;
        }
    }
    if (!okay) {
        std::cerr << "Didn't clean exit !" << std::endl;
    }
}
