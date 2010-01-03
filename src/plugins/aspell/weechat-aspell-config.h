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


#ifndef __WEECHAT_ASPELL_CONFIG_H
#define __WEECHAT_ASPELL_CONFIG_H 1

#define ASPELL_CONFIG_NAME "aspell"


extern struct t_config_option *weechat_aspell_config_look_color;

extern struct t_config_option *weechat_aspell_config_check_commands;
extern struct t_config_option *weechat_aspell_config_check_default_dict;
extern struct t_config_option *weechat_aspell_config_check_during_search;
extern struct t_config_option *weechat_aspell_config_check_real_time;
extern struct t_config_option *weechat_aspell_config_check_word_min_length;

extern char **weechat_aspell_commands_to_check;
extern int weechat_aspell_count_commands_to_check;
extern int *weechat_aspell_length_commands_to_check;

extern struct t_config_option *weechat_aspell_config_get_dict (const char *name);
extern int weechat_aspell_config_set_dict (const char *name, const char *value);
extern int weechat_aspell_config_init ();
extern int weechat_aspell_config_read ();
extern int weechat_aspell_config_write ();
extern void weechat_aspell_config_free ();

#endif /* weechat-aspell-config.h */
