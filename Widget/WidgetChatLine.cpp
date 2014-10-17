/**
    gTox a GTK-based tox-client - https://github.com/KoKuToru/gTox.git

    Copyright (C) 2014  Luca Béla Palkovics
    Copyright (C) 2014  Maurice Mohlek

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
#include "WidgetChatLine.h"
#include "Generated/icon.h"
WidgetChatLine::WidgetChatLine(bool left_side) : m_side(left_side) {
    this->set_halign(m_side?Gtk::Align::ALIGN_START:Gtk::Align::ALIGN_END);

    auto hbox = Gtk::manage(new Gtk::HBox());
    auto frame = Gtk::manage(new Gtk::Frame());
    add(*hbox);
    if (m_side) {
        hbox->pack_start(m_avatar);
        m_avatar.set_valign(Gtk::ALIGN_START);
        m_avatar.set_halign(Gtk::ALIGN_END);
    }
    hbox->add(*frame);
    if (!m_side) {
        hbox->pack_end(m_avatar);
        m_avatar.set_valign(Gtk::ALIGN_END);
        m_avatar.set_halign(Gtk::ALIGN_START);
    }
    frame->add(m_msg);

    //m_msg.set_size_request(500);
    m_msg.set_selectable(true);
    m_msg.set_line_wrap(true);
    m_msg.set_line_wrap_mode(Pango::WRAP_WORD_CHAR);
    m_msg.set_justify(Gtk::JUSTIFY_LEFT);

    m_avatar.set_name("Avatar");
    m_avatar.set_size_request(64, 0);

    frame->set_name(m_side?"WidgetChatLineLeft":"WidgetChatLineRight");

    show_all();
}

WidgetChatLine::~WidgetChatLine(){
}

bool WidgetChatLine::get_side() {
    return m_side;
}

void WidgetChatLine::add_line(unsigned long long timestamp, const Glib::ustring& message) {
    //TODO display timestmap
    (void)timestamp;

    //Todo use text view ?
    if (m_msg.get_text().bytes() != 0) {
        m_msg.set_text(m_msg.get_text() + "\n");
    }
    m_msg.set_text(m_msg.get_text() + message);
}

#include <iostream>
void WidgetChatLine::on_size_allocate(Gtk::Allocation& allocation) {
    int h = allocation.get_height();
    auto ic = ICON::load_icon(ICON::avatar);
    if (h < ic->get_height()) {
        std::cout << "h:" << h << std::endl;
        m_avatar.set_size_request(64, h);
        m_avatar.clear();
        m_avatar.property_pixbuf() = ICON::load_icon(ICON::avatar)->scale_simple(h, h, Gdk::INTERP_BILINEAR);
        //force allocation (is this a bug ? why do I need to do this =?
        Gtk::Allocation al = m_avatar.get_allocation();
        al.set_y(al.get_height()-h);
        al.set_height(h);
    } else {
        //TODO check if already done..
        h = ic->get_height();
        m_avatar.set_size_request(64, h);
        m_avatar.clear();
        m_avatar.property_pixbuf() = ICON::load_icon(ICON::avatar);
    }
    //Important to do last !
    Gtk::Box::on_size_allocate(allocation);
}
