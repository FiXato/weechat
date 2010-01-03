/*
 * Copyright (c) 2003-2010 by FlashCode <flashcode@flashtux.org>
 * See README for License detail, AUTHORS for developers list.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
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


#ifndef __WEECHAT_RELAY_H
#define __WEECHAT_RELAY_H 1

#define weechat_plugin weechat_relay_plugin
#define RELAY_PLUGIN_NAME "relay"

extern struct t_weechat_plugin *weechat_relay_plugin;

/* relay protocol */

enum t_relay_protocol
{
    RELAY_PROTOCOL_WEECHAT = 0,        /* WeeChat protocol                  */
    RELAY_PROTOCOL_IRC,                /* IRC protocol (IRC proxy)          */
    /* number of relay protocols */
    RELAY_NUM_PROTOCOLS,
};

#define RELAY_COLOR_CHAT weechat_color("chat")
#define RELAY_COLOR_CHAT_HOST weechat_color("chat_host")
#define RELAY_COLOR_CHAT_BUFFER weechat_color("chat_buffer")

extern char *relay_protocol_string[];

extern int relay_protocol_search (const char *name);

#endif /* relay.h */
