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


#ifndef __WEECHAT_IRC_MSGBUFFER_H
#define __WEECHAT_IRC_MSGBUFFER_H 1

enum t_irc_msgbuffer_target
{
    IRC_MSGBUFFER_TARGET_WEECHAT = 0,
    IRC_MSGBUFFER_TARGET_CURRENT,
    IRC_MSGBUFFER_TARGET_PRIVATE,
    /* number of msgbuffer targets */
    IRC_MSGBUFFER_NUM_TARGETS,
};

struct t_irc_server;

struct t_irc_msgbuffer
{
    char *message;                      /* IRC message                       */
    enum t_irc_msgbuffer_target target; /* target buffer                     */
};

extern const char *irc_msgbuffer_get_string (int target);
extern struct t_gui_buffer *irc_msgbuffer_get_target_buffer (struct t_irc_server *server,
                                                             const char *nick,
                                                             const char *message,
                                                             const char *alias);

#endif /* irc-msgbuffer.h */
