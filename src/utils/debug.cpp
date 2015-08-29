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
#include <thread>
#include <mutex>
#include <iostream>
#include <iomanip>
#ifndef NO_UNWIND
#define UNW_LOCAL_ONLY
#include <libunwind.h>
#else
#define unw_word_t int
#endif
#include <unistd.h>
#include <stdio.h>
#include <glibmm.h>
using namespace utils;

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

bool caller_symbol(std::string& out_symbol, unw_word_t& out_pc, unw_word_t& out_offset) {
#ifndef NO_UNWIND
    unw_cursor_t cursor;
    unw_context_t context;

    // Initialize cursor to current frame for local unwinding.
    unw_getcontext(&context);
    unw_init_local(&cursor, &context);

    if (!(unw_step(&cursor) > 0)) {
        return false;
    }
    if (!(unw_step(&cursor) > 0)) {
        return false;
    }

    // Get symbol name
    char sym[1024];
    if (unw_get_proc_name(&cursor, sym, sizeof(sym), &out_offset) != 0) {
        return false;
    }

    // Get demangled symbol name
    int status;
    char* demangled = abi::__cxa_demangle(sym, nullptr, nullptr, &status);
    if (status == 0) {
        out_symbol = demangled;
    } else {
        out_symbol = sym;
    }
    free(demangled);

    unw_get_reg(&cursor, UNW_REG_IP, &out_pc);
#endif
    return true;
}

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

void print_symbol_info(int depth, char m, const std::string& symbol, unw_word_t pc, unw_word_t offset) {
    static auto is_terminal = isatty(fileno(stdout)) == 1;

    if (depth > 0) {
        std::clog << std::setfill(' ');
        std::clog << std::setw(depth) << " ";
    }

    //thread id
    std::clog << m << "[";
    if (is_terminal) {
        std::clog << KMAG;
    }
    std::clog << "0x";
    std::clog << std::setw(sizeof(void*)*2) << std::setfill('0') << std::right << std::hex;
    std::clog << std::this_thread::get_id();
    if (is_terminal) {
        std::clog << KNRM;
    }
    std::clog << "] ";

    //function addr
    if (is_terminal) {
        std::clog << KRED;
    }
    std::clog << "0x";
    std::clog << std::setw(sizeof(void*)*2) << std::setfill('0') << std::right << std::hex;
    std::clog << pc;
    if (is_terminal) {
        std::clog << KNRM;
    }
    std::clog << ": ";

    //function name
    if (is_terminal) {
        std::clog << KYEL;
    }
    std::clog << symbol;

    //function position
    if (is_terminal) {
        std::clog << KRED;
    }
    std::clog << "+0x";
    std::clog << std::setw(sizeof(short)*2) << std::setfill('0') << std::right << std::hex;
    std::clog << offset;
    if (is_terminal) {
        std::clog << KNRM;
    }
    std::clog << std::endl;
}

thread_local unsigned depth = 0;
static std::mutex log_mtx;

log::log() {
    static bool enable_log = Glib::getenv("GTOX_LOG") == "1";
    if (!enable_log) {
        return;
    }
    //get function name
    std::string symbol;
    unw_word_t pc = 0;
    unw_word_t offset = 0;
    if (!caller_symbol(symbol, pc, offset)) {
        symbol = "Couldn't get backtrace";
    }

    //print info
    std::lock_guard<std::mutex> lg(log_mtx);
    print_symbol_info(depth++, '+', symbol, pc, offset);
}

log::~log() {
    static bool enable_log = Glib::getenv("GTOX_LOG") == "1";
    if (!enable_log) {
        return;
    }
    //get function name
    std::string symbol;
    unw_word_t pc = 0;
    unw_word_t offset = 0;
    if (!caller_symbol(symbol, pc, offset)) {
        symbol = "Couldn't get backtrace";
    }

    //print info
    std::lock_guard<std::mutex> lg(log_mtx);
    print_symbol_info(--depth, '-', symbol, pc, offset);
}
