/*
 * Copyright (c) 2003-2008 by FlashCode <flashcode@flashtux.org>
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

/* weechat-python-api.c: Python API functions */

#undef _

#include <Python.h>

#include "../../weechat-plugin.h"
#include "../script.h"
#include "../script-api.h"
#include "../script-callback.h"
#include "weechat-python.h"


#define PYTHON_RETURN_OK return Py_BuildValue ("i", 1);
#define PYTHON_RETURN_ERROR return Py_BuildValue ("i", 0);
#define PYTHON_RETURN_EMPTY \
    Py_INCREF(Py_None);     \
    return Py_None;
#define PYTHON_RETURN_STRING(__string)             \
    if (__string)                                  \
        return Py_BuildValue ("s", __string);      \
    return Py_BuildValue ("s", "")
#define PYTHON_RETURN_STRING_FREE(__string)        \
    if (__string)                                  \
    {                                              \
        object = Py_BuildValue ("s", __string);    \
        free (__string);                           \
        return object;                             \
    }                                              \
    return Py_BuildValue ("s", "")
#define PYTHON_RETURN_INT(__int)                \
    return Py_BuildValue("i", __int);


/*
 * weechat_python_api_register: startup function for all WeeChat Python scripts
 */

static PyObject *
weechat_python_api_register (PyObject *self, PyObject *args)
{
    char *name, *author, *version, *license, *shutdown_func, *description;
    char *charset;
    
    /* make C compiler happy */
    (void) self;
    
    python_current_script = NULL;
    
    name = NULL;
    author = NULL;
    version = NULL;
    license = NULL;
    description = NULL;
    shutdown_func = NULL;
    charset = NULL;
    
    if (!PyArg_ParseTuple (args, "sssssss", &name, &author, &version,
                           &license, &description, &shutdown_func, &charset))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("register");
        PYTHON_RETURN_ERROR;
    }
    
    if (script_search (weechat_python_plugin, python_scripts, name))
    {
        /* error: another scripts already exists with this name! */
        weechat_printf (NULL,
                        weechat_gettext ("%s%s: unable to register script "
                                         "\"%s\" (another script already "
                                         "exists with this name)"),
                        weechat_prefix ("error"), "python", name);
        PYTHON_RETURN_ERROR;
    }
    
    /* register script */
    python_current_script = script_add (weechat_python_plugin,
                                        &python_scripts,
                                        (python_current_script_filename) ?
                                        python_current_script_filename : "",
                                        name, author, version, license,
                                        description, shutdown_func, charset);
    if (python_current_script)
    {
        weechat_printf (NULL,
                        weechat_gettext ("%s: registered script \"%s\", "
                                         "version %s (%s)"),
                        "python", name, version, description);
    }
    else
    {
        PYTHON_RETURN_ERROR;
    }
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_charset_set: set script charset
 */

static PyObject *
weechat_python_api_charset_set (PyObject *self, PyObject *args)
{
    char *charset;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("charset_set");
        PYTHON_RETURN_ERROR;
    }
    
    charset = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &charset))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("charset_set");
        PYTHON_RETURN_ERROR;
    }
    
    script_api_charset_set (python_current_script,
                            charset);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_iconv_to_internal: convert string to internal WeeChat charset
 */

static PyObject *
weechat_python_api_iconv_to_internal (PyObject *self, PyObject *args)
{
    char *charset, *string, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("iconv_to_internal");
        PYTHON_RETURN_EMPTY;
    }
    
    charset = NULL;
    string = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &charset, &string))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("iconv_to_internal");
        PYTHON_RETURN_EMPTY;
    }
    
    result = weechat_iconv_to_internal (charset, string);
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_iconv_from_internal: convert string from WeeChat internal
 *                                         charset to another one
 */

static PyObject *
weechat_python_api_iconv_from_internal (PyObject *self, PyObject *args)
{
    char *charset, *string, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("iconv_from_internal");
        PYTHON_RETURN_EMPTY;
    }
    
    charset = NULL;
    string = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &charset, &string))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("iconv_from_internal");
        PYTHON_RETURN_EMPTY;
    }
    
    result = weechat_iconv_from_internal (charset, string);
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_gettext: get translated string
 */

static PyObject *
weechat_python_api_gettext (PyObject *self, PyObject *args)
{
    char *string, *result;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("gettext");
        PYTHON_RETURN_EMPTY;
    }
    
    string = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &string))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("gettext");
        PYTHON_RETURN_EMPTY;
    }
    
    result = weechat_gettext (string);
    
    PYTHON_RETURN_STRING(result);
}

/*
 * weechat_python_api_ngettext: get translated string with plural form
 */

static PyObject *
weechat_python_api_ngettext (PyObject *self, PyObject *args)
{
    char *single, *plural, *result;
    int count;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("ngettext");
        PYTHON_RETURN_EMPTY;
    }
    
    single = NULL;
    plural = NULL;
    count = 0;
    
    if (!PyArg_ParseTuple (args, "ssi", &single, &plural, &count))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("ngettext");
        PYTHON_RETURN_EMPTY;
    }
    
    result = weechat_ngettext (single, plural, count);
    
    PYTHON_RETURN_STRING(result);
}

/*
 * weechat_python_api_mkdir_home: create a directory in WeeChat home
 */

static PyObject *
weechat_python_api_mkdir_home (PyObject *self, PyObject *args)
{
    char *directory;
    int mode;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("mkdir_home");
        PYTHON_RETURN_ERROR;
    }
    
    directory = NULL;
    mode = 0;
    
    if (!PyArg_ParseTuple (args, "si", &directory, &mode))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("mkdir_home");
        PYTHON_RETURN_ERROR;
    }
    
    if (weechat_mkdir_home (directory, mode))
        PYTHON_RETURN_OK;

    PYTHON_RETURN_ERROR;
}

/*
 * weechat_python_api_mkdir: create a directory
 */

static PyObject *
weechat_python_api_mkdir (PyObject *self, PyObject *args)
{
    char *directory;
    int mode;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("mkdir");
        PYTHON_RETURN_ERROR;
    }
    
    directory = NULL;
    mode = 0;
    
    if (!PyArg_ParseTuple (args, "si", &directory, &mode))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("mkdir");
        PYTHON_RETURN_ERROR;
    }
    
    if (weechat_mkdir (directory, mode))
        PYTHON_RETURN_OK;
    
    PYTHON_RETURN_ERROR;
}

/*
 * weechat_python_api_list_new: create a new list
 */

static PyObject *
weechat_python_api_list_new (PyObject *self, PyObject *args)
{
    char *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    (void) args;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("list_new");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_list_new ());
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_list_add: add a string to list
 */

static PyObject *
weechat_python_api_list_add (PyObject *self, PyObject *args)
{
    char *weelist, *data, *where, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("list_add");
        PYTHON_RETURN_EMPTY;
    }
    
    weelist = NULL;
    data = NULL;
    where = NULL;
    
    if (!PyArg_ParseTuple (args, "sss", &weelist, &data, &where))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("list_add");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_list_add (script_str2ptr (weelist),
                                               data,
                                               where));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_list_search: search a string in list
 */

static PyObject *
weechat_python_api_list_search (PyObject *self, PyObject *args)
{
    char *weelist, *data, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("list_search");
        PYTHON_RETURN_EMPTY;
    }
    
    weelist = NULL;
    data = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &weelist, &data))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("list_search");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_list_search (script_str2ptr (weelist),
                                                  data));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_list_casesearch: search a string in list (ignore case)
 */

static PyObject *
weechat_python_api_list_casesearch (PyObject *self, PyObject *args)
{
    char *weelist, *data, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("list_casesearch");
        PYTHON_RETURN_EMPTY;
    }
    
    weelist = NULL;
    data = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &weelist, &data))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("list_casesearch");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_list_casesearch (script_str2ptr (weelist),
                                                      data));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_list_get: get item by position
 */

static PyObject *
weechat_python_api_list_get (PyObject *self, PyObject *args)
{
    char *weelist, *result;
    int position;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("list_get");
        PYTHON_RETURN_EMPTY;
    }
    
    weelist = NULL;
    position = 0;
    
    if (!PyArg_ParseTuple (args, "si", &weelist, &position))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("list_get");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_list_get (script_str2ptr (weelist),
                                               position));
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_list_set: set new value for item
 */

static PyObject *
weechat_python_api_list_set (PyObject *self, PyObject *args)
{
    char *item, *new_value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("list_set");
        PYTHON_RETURN_ERROR;
    }
    
    item = NULL;
    new_value = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &item, &new_value))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("list_set");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_list_set (script_str2ptr (item),
                      new_value);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_list_next: get next item
 */

static PyObject *
weechat_python_api_list_next (PyObject *self, PyObject *args)
{
    char *item, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("list_next");
        PYTHON_RETURN_EMPTY;
    }
    
    item = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &item))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("list_next");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_list_next (script_str2ptr (item)));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_list_prev: get previous item
 */

static PyObject *
weechat_python_api_list_prev (PyObject *self, PyObject *args)
{
    char *item, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("list_prev");
        PYTHON_RETURN_EMPTY;
    }
    
    item = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &item))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("list_prev");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_list_prev (script_str2ptr (item)));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_list_string: get string value of item
 */

static PyObject *
weechat_python_api_list_string (PyObject *self, PyObject *args)
{
    char *item, *result;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("list_string");
        PYTHON_RETURN_EMPTY;
    }
    
    item = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &item))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("list_string");
        PYTHON_RETURN_EMPTY;
    }
    
    result = weechat_list_string (script_str2ptr (item));
    
    PYTHON_RETURN_STRING(result);
}

/*
 * weechat_python_api_list_size: get number of elements in list
 */

static PyObject *
weechat_python_api_list_size (PyObject *self, PyObject *args)
{
    char *weelist;
    int size;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("list_size");
        PYTHON_RETURN_INT(0);
    }
    
    weelist = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &weelist))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("list_size");
        PYTHON_RETURN_INT(0);
    }
    
    size = weechat_list_size (script_str2ptr (weelist));
    
    PYTHON_RETURN_INT(size);
}

/*
 * weechat_python_api_list_remove: remove item from list
 */

static PyObject *
weechat_python_api_list_remove (PyObject *self, PyObject *args)
{
    char *weelist, *item;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("list_remove");
        PYTHON_RETURN_ERROR;
    }
    
    weelist = NULL;
    item = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &weelist, &item))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("list_remove");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_list_remove (script_str2ptr (weelist),
                         script_str2ptr (item));
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_list_remove_all: remove all items from list
 */

static PyObject *
weechat_python_api_list_remove_all (PyObject *self, PyObject *args)
{
    char *weelist;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("list_remove_all");
        PYTHON_RETURN_ERROR;
    }
    
    weelist = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &weelist))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("list_remove_all");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_list_remove_all (script_str2ptr (weelist));
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_list_free: free list
 */

static PyObject *
weechat_python_api_list_free (PyObject *self, PyObject *args)
{
    char *weelist;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("list_free");
        PYTHON_RETURN_ERROR;
    }
    
    weelist = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &weelist))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("list_free");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_list_free (script_str2ptr (weelist));
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_config_reload_cb: callback for config reload
 */

int
weechat_python_api_config_reload_cb (void *data,
                                     struct t_config_file *config_file)
{
    struct t_script_callback *script_callback;
    char *python_argv[2];
    int *rc, ret;
    
    script_callback = (struct t_script_callback *)data;

    if (script_callback->function && script_callback->function[0])
    {
        python_argv[0] = script_ptr2str (config_file);
        python_argv[1] = NULL;
        
        rc = (int *) weechat_python_exec (script_callback->script,
                                          WEECHAT_SCRIPT_EXEC_INT,
                                          script_callback->function,
                                          python_argv);
        
        if (!rc)
            ret = WEECHAT_RC_ERROR;
        else
        {
            ret = *rc;
            free (rc);
        }
        if (python_argv[0])
            free (python_argv[0]);
        
        return ret;
    }
    
    return 0;
}

/*
 * weechat_python_api_config_new: create a new configuration file
 */

static PyObject *
weechat_python_api_config_new (PyObject *self, PyObject *args)
{
    char *name, *function, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_new");
        PYTHON_RETURN_EMPTY;
    }
    
    name = NULL;
    function = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &name, &function))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_new");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (script_api_config_new (weechat_python_plugin,
                                                    python_current_script,
                                                    name,
                                                    &weechat_python_api_config_reload_cb,
                                                    function));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_config_read_cb: callback for reading option in section
 */

void
weechat_python_api_config_read_cb (void *data,
                                   struct t_config_file *config_file,
                                   char *option_name, char *value)
{
    struct t_script_callback *script_callback;
    char *python_argv[4];
    int *rc;
    
    script_callback = (struct t_script_callback *)data;

    if (script_callback->function && script_callback->function[0])
    {
        python_argv[0] = script_ptr2str (config_file);
        python_argv[1] = option_name;
        python_argv[2] = value;
        python_argv[3] = NULL;
        
        rc = (int *) weechat_python_exec (script_callback->script,
                                          WEECHAT_SCRIPT_EXEC_INT,
                                          script_callback->function,
                                          python_argv);
        
        if (rc)
            free (rc);
        if (python_argv[0])
            free (python_argv[0]);
    }
}

/*
 * weechat_python_api_config_section_write_cb: callback for writing section
 */

void
weechat_python_api_config_section_write_cb (void *data,
                                            struct t_config_file *config_file,
                                            char *section_name)
{
    struct t_script_callback *script_callback;
    char *python_argv[3];
    int *rc;
    
    script_callback = (struct t_script_callback *)data;

    if (script_callback->function && script_callback->function[0])
    {
        python_argv[0] = script_ptr2str (config_file);
        python_argv[1] = section_name;
        python_argv[2] = NULL;
        
        rc = (int *) weechat_python_exec (script_callback->script,
                                          WEECHAT_SCRIPT_EXEC_INT,
                                          script_callback->function,
                                          python_argv);
        
        if (rc)
            free (rc);
        if (python_argv[0])
            free (python_argv[0]);
    }
}

/*
 * weechat_python_api_config_section_write_default_cb: callback for writing
 *                                                     default values for section
 */

void
weechat_python_api_config_section_write_default_cb (void *data,
                                                    struct t_config_file *config_file,
                                                    char *section_name)
{
    struct t_script_callback *script_callback;
    char *python_argv[3];
    int *rc;
    
    script_callback = (struct t_script_callback *)data;

    if (script_callback->function && script_callback->function[0])
    {
        python_argv[0] = script_ptr2str (config_file);
        python_argv[1] = section_name;
        python_argv[2] = NULL;
        
        rc = (int *) weechat_python_exec (script_callback->script,
                                          WEECHAT_SCRIPT_EXEC_INT,
                                          script_callback->function,
                                          python_argv);
        
        if (rc)
            free (rc);
        if (python_argv[0])
            free (python_argv[0]);
    }
}

/*
 * weechat_python_api_config_section_create_option_cb: callback to create an option
 */

int
weechat_python_api_config_section_create_option_cb (void *data,
                                                    struct t_config_file *config_file,
                                                    struct t_config_section *section,
                                                    char *option_name,
                                                    char *value)
{
    struct t_script_callback *script_callback;
    char *python_argv[5];
    int *rc, ret;
    
    script_callback = (struct t_script_callback *)data;
    
    if (script_callback->function && script_callback->function[0])
    {
        python_argv[0] = script_ptr2str (config_file);
        python_argv[1] = script_ptr2str (section);
        python_argv[2] = option_name;
        python_argv[3] = value;
        python_argv[4] = NULL;

        rc = (int *) weechat_python_exec (script_callback->script,
                                          WEECHAT_SCRIPT_EXEC_INT,
                                          script_callback->function,
                                          python_argv);
        
        if (!rc)
            ret = WEECHAT_RC_ERROR;
        else
        {
            ret = *rc;
            free (rc);
        }
        if (python_argv[0])
            free (python_argv[0]);
        if (python_argv[1])
            free (python_argv[1]);
        
        return ret;
    }
    
    return 0;
}

/*
 * weechat_python_api_config_new_section: create a new section in configuration file
 */

static PyObject *
weechat_python_api_config_new_section (PyObject *self, PyObject *args)
{
    char *config_file, *name, *function_read, *function_write;
    char *function_write_default, *function_create_option;
    char *result;
    int user_can_add_options, user_can_delete_options;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_new_section");
        PYTHON_RETURN_EMPTY;
    }
    
    config_file = NULL;
    name = NULL;
    user_can_add_options = 0;
    user_can_delete_options = 0;
    function_read = NULL;
    function_write = NULL;
    function_write_default = NULL;
    function_create_option = NULL;
    
    if (!PyArg_ParseTuple (args, "ssiissss", &config_file, &name,
                           &user_can_add_options, &user_can_delete_options,
                           &function_read, &function_write,
                           &function_write_default, &function_create_option))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_new_section");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (script_api_config_new_section (weechat_python_plugin,
                                                            python_current_script,
                                                            script_str2ptr (config_file),
                                                            name,
                                                            user_can_add_options,
                                                            user_can_delete_options,
                                                            &weechat_python_api_config_read_cb,
                                                            function_read,
                                                            &weechat_python_api_config_section_write_cb,
                                                            function_write,
                                                            &weechat_python_api_config_section_write_default_cb,
                                                            function_write_default,
                                                            &weechat_python_api_config_section_create_option_cb,
                                                            function_create_option));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_config_search_section: search section in configuration file
 */

static PyObject *
weechat_python_api_config_search_section (PyObject *self, PyObject *args)
{
    char *config_file, *section_name, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_search_section");
        PYTHON_RETURN_EMPTY;
    }
    
    config_file = NULL;
    section_name = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &config_file, &section_name))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_search_section");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_config_search_section (script_str2ptr (config_file),
                                                            section_name));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_config_option_change_cb: callback for option changed
 */

void
weechat_python_api_config_option_change_cb (void *data)
{
    struct t_script_callback *script_callback;
    char *python_argv[1];
    int *rc;
    
    script_callback = (struct t_script_callback *)data;

    if (script_callback->function && script_callback->function[0])
    {
        python_argv[0] = NULL;
        
        rc = (int *) weechat_python_exec (script_callback->script,
                                          WEECHAT_SCRIPT_EXEC_INT,
                                          script_callback->function,
                                          python_argv);
        
        if (rc)
            free (rc);
    }
}

/*
 * weechat_python_api_config_new_option: create a new option in section
 */

static PyObject *
weechat_python_api_config_new_option (PyObject *self, PyObject *args)
{
    char *config_file, *section, *name, *type, *description, *string_values;
    char *default_value, *function, *result;
    int min, max;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_new_option");
        PYTHON_RETURN_EMPTY;
    }
    
    config_file = NULL;
    section = NULL;
    name = NULL;
    type = NULL;
    description = NULL;
    string_values = NULL;
    default_value = NULL;
    function = NULL;
    
    if (!PyArg_ParseTuple (args, "ssssssiiss", &config_file, &section, &name,
                           &type, &description, &string_values, &min, &max,
                           &default_value, &function))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_new_option");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (script_api_config_new_option (weechat_python_plugin,
                                                           python_current_script,
                                                           script_str2ptr (config_file),
                                                           script_str2ptr (section),
                                                           name,
                                                           type,
                                                           description,
                                                           string_values,
                                                           min,
                                                           max,
                                                           default_value,
                                                           &weechat_python_api_config_option_change_cb,
                                                           function));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_config_search_option: search option in configuration file or section
 */

static PyObject *
weechat_python_api_config_search_option (PyObject *self, PyObject *args)
{
    char *config_file, *section, *option_name, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_search_option");
        PYTHON_RETURN_EMPTY;
    }
    
    config_file = NULL;
    section = NULL;
    option_name = NULL;
    
    if (!PyArg_ParseTuple (args, "sss", &config_file, &section, &option_name))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_search_option");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_config_search_option (script_str2ptr (config_file),
                                                           script_str2ptr (section),
                                                           option_name));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_config_string_to_boolean: return boolean value of a string
 */

static PyObject *
weechat_python_api_config_string_to_boolean (PyObject *self, PyObject *args)
{
    char *text;
    int value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_string_to_boolean");
        PYTHON_RETURN_INT(0);
    }
    
    text = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &text))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_string_to_boolean");
        PYTHON_RETURN_INT(0);
    }
    
    value = weechat_config_string_to_boolean (text);
    
    PYTHON_RETURN_INT(value);
}

/*
 * weechat_python_api_config_option_reset: reset an option with default value
 */

static PyObject *
weechat_python_api_config_option_reset (PyObject *self, PyObject *args)
{
    char *option;
    int run_callback, rc;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_option_reset");
        PYTHON_RETURN_INT(0);
    }
    
    option = NULL;
    run_callback = 0;
    
    if (!PyArg_ParseTuple (args, "si", &option, &run_callback))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_option_reset");
        PYTHON_RETURN_INT(0);
    }
    
    rc = weechat_config_option_reset (script_str2ptr (option),
                                      run_callback);
    
    PYTHON_RETURN_INT(rc);
}

/*
 * weechat_python_api_config_option_set: set new value for option
 */

static PyObject *
weechat_python_api_config_option_set (PyObject *self, PyObject *args)
{
    char *option, *new_value;
    int run_callback, rc;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_option_set");
        PYTHON_RETURN_INT(0);
    }
    
    option = NULL;
    new_value = NULL;
    run_callback = 0;
    
    if (!PyArg_ParseTuple (args, "ssi", &option, &new_value, &run_callback))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_option_set");
        PYTHON_RETURN_INT(0);
    }
    
    rc = weechat_config_option_set (script_str2ptr (option),
                                    new_value,
                                    run_callback);
    
    PYTHON_RETURN_INT(rc);
}

/*
 * weechat_python_api_config_option_rename: rename an option
 */

static PyObject *
weechat_python_api_config_option_rename (PyObject *self, PyObject *args)
{
    char *option, *new_name;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_option_rename");
        PYTHON_RETURN_ERROR;
    }
    
    option = NULL;
    new_name = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &option, &new_name))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_option_rename");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_config_option_rename (script_str2ptr (option),
                                  new_name);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_config_boolean: return boolean value of option
 */

static PyObject *
weechat_python_api_config_boolean (PyObject *self, PyObject *args)
{
    char *option;
    int value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_boolean");
        PYTHON_RETURN_INT(0);
    }
    
    option = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &option))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_boolean");
        PYTHON_RETURN_INT(0);
    }
    
    value = weechat_config_boolean (script_str2ptr (option));
    
    PYTHON_RETURN_INT(value);
}

/*
 * weechat_python_api_config_integer: return integer value of option
 */

static PyObject *
weechat_python_api_config_integer (PyObject *self, PyObject *args)
{
    char *option;
    int value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_integer");
        PYTHON_RETURN_INT(0);
    }
    
    option = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &option))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_integer");
        PYTHON_RETURN_INT(0);
    }
    
    value = weechat_config_integer (script_str2ptr (option));
    
    PYTHON_RETURN_INT(value);
}

/*
 * weechat_python_api_config_string: return string value of option
 */

static PyObject *
weechat_python_api_config_string (PyObject *self, PyObject *args)
{
    char *option, *value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_string");
        PYTHON_RETURN_EMPTY;
    }
    
    option = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &option))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_string");
        PYTHON_RETURN_EMPTY;
    }
    
    value = weechat_config_string (script_str2ptr (option));
    
    PYTHON_RETURN_STRING(value);
}

/*
 * weechat_python_api_config_color: return color value of option
 */

static PyObject *
weechat_python_api_config_color (PyObject *self, PyObject *args)
{
    char *option;
    int value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_color");
        PYTHON_RETURN_INT(0);
    }
    
    option = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &option))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_color");
        PYTHON_RETURN_INT(0);
    }
    
    value = weechat_config_color (script_str2ptr (option));
    
    PYTHON_RETURN_INT(value);
}

/*
 * weechat_python_api_config_write_line: write a line in configuration file
 */

static PyObject *
weechat_python_api_config_write_line (PyObject *self, PyObject *args)
{
    char *config_file, *option_name, *value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_write_line");
        PYTHON_RETURN_ERROR;
    }
    
    config_file = NULL;
    option_name = NULL;
    value = NULL;
    
    if (!PyArg_ParseTuple (args, "sss", &config_file, &option_name, &value))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_write_line");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_config_write_line (script_str2ptr (config_file),
                               option_name,
                               "%s",
                               value);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_config_write: write configuration file
 */

static PyObject *
weechat_python_api_config_write (PyObject *self, PyObject *args)
{
    char *config_file;
    int rc;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_write");
        PYTHON_RETURN_INT(-1);
    }
    
    config_file = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &config_file))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_write");
        PYTHON_RETURN_INT(-1);
    }
    
    rc = weechat_config_write (script_str2ptr (config_file));
    
    PYTHON_RETURN_INT(rc);
}

/*
 * weechat_python_api_config_read: read configuration file
 */

static PyObject *
weechat_python_api_config_read (PyObject *self, PyObject *args)
{
    char *config_file;
    int rc;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_read");
        PYTHON_RETURN_INT(-1);
    }
    
    config_file = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &config_file))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_read");
        PYTHON_RETURN_INT(-1);
    }
    
    rc = weechat_config_read (script_str2ptr (config_file));
    
    PYTHON_RETURN_INT(rc);
}

/*
 * weechat_python_api_config_reload: reload configuration file
 */

static PyObject *
weechat_python_api_config_reload (PyObject *self, PyObject *args)
{
    char *config_file;
    int rc;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_reload");
        PYTHON_RETURN_INT(-1);
    }
    
    config_file = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &config_file))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_reload");
        PYTHON_RETURN_INT(-1);
    }
    
    rc = weechat_config_reload (script_str2ptr (config_file));
    
    PYTHON_RETURN_INT(rc);
}

/*
 * weechat_python_api_config_free: free configuration file
 */

static PyObject *
weechat_python_api_config_free (PyObject *self, PyObject *args)
{
    char *config_file;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_free");
        PYTHON_RETURN_ERROR;
    }
    
    config_file = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &config_file))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_free");
        PYTHON_RETURN_ERROR;
    }
    
    script_api_config_free (weechat_python_plugin,
                            python_current_script,
                            script_str2ptr (config_file));
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_config_get: get config option
 */

static PyObject *
weechat_python_api_config_get (PyObject *self, PyObject *args)
{
    char *option, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_get");
        PYTHON_RETURN_EMPTY;
    }
    
    option = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &option))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_get");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_config_get (option));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_config_get_plugin: get value of a plugin option
 */

static PyObject *
weechat_python_api_config_get_plugin (PyObject *self, PyObject *args)
{
    char *option, *value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_get_plugin");
        PYTHON_RETURN_EMPTY;
    }
    
    option = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &option))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_get_plugin");
        PYTHON_RETURN_EMPTY;
    }
    
    value = script_api_config_get_plugin (weechat_python_plugin,
                                          python_current_script,
                                          option);
    
    PYTHON_RETURN_STRING(value);
}

/*
 * weechat_python_api_config_set_plugin: set value of a plugin option
 */

static PyObject *
weechat_python_api_config_set_plugin (PyObject *self, PyObject *args)
{
    char *option, *value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("config_set_plugin");
        PYTHON_RETURN_ERROR;
    }
    
    option = NULL;
    value = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &option, &value))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("config_set_plugin");
        PYTHON_RETURN_ERROR;
    }
    
    if (script_api_config_set_plugin (weechat_python_plugin,
                                      python_current_script,
                                      option,
                                      value))
        PYTHON_RETURN_OK;
    
    PYTHON_RETURN_ERROR;
}

/*
 * weechat_python_api_prefix: get a prefix, used for display
 */

static PyObject *
weechat_python_api_prefix (PyObject *self, PyObject *args)
{
    char *prefix, *result;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("prefix");
        PYTHON_RETURN_EMPTY;
    }
    
    prefix = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &prefix))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("prefix");
        PYTHON_RETURN_EMPTY;
    }
    
    result = weechat_prefix (prefix);
    
    PYTHON_RETURN_STRING(result);
}

/*
 * weechat_python_api_color: get a color code, used for display
 */

static PyObject *
weechat_python_api_color (PyObject *self, PyObject *args)
{
    char *color, *result;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("color");
        PYTHON_RETURN_EMPTY;
    }
    
    color = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &color))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("color");
        PYTHON_RETURN_EMPTY;
    }
    
    result = weechat_color (color);
    
    PYTHON_RETURN_STRING(result);
}

/*
 * weechat_python_api_prnt: print message in a buffer
 */

static PyObject *
weechat_python_api_prnt (PyObject *self, PyObject *args)
{
    char *buffer, *message;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("prnt");
        PYTHON_RETURN_ERROR;
    }
    
    buffer = NULL;
    message = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &buffer, &message))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("prnt");
        PYTHON_RETURN_ERROR;
    }
    
    script_api_printf (weechat_python_plugin,
                       python_current_script,
                       script_str2ptr (buffer),
                       "%s", message);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_prnt_date_tags: print message in a buffer with optional
 *                                    date and tags
 */

static PyObject *
weechat_python_api_prnt_date_tags (PyObject *self, PyObject *args)
{
    char *buffer, *tags, *message;
    int date;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("prnt_date_tags");
        PYTHON_RETURN_ERROR;
    }
    
    buffer = NULL;
    date = 0;
    tags = NULL;
    message = NULL;
    
    if (!PyArg_ParseTuple (args, "siss", &buffer, &time, &tags, &message))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("prnt_date_tags");
        PYTHON_RETURN_ERROR;
    }
    
    script_api_printf_date_tags (weechat_python_plugin,
                                 python_current_script,
                                 script_str2ptr (buffer),
                                 date,
                                 tags,
                                 "%s", message);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_prnt_y: print message in a buffer with free content
 */

static PyObject *
weechat_python_api_prnt_y (PyObject *self, PyObject *args)
{
    char *buffer, *message;
    int y;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("prnt_y");
        PYTHON_RETURN_ERROR;
    }
    
    buffer = NULL;
    y = 0;
    message = NULL;
    
    if (!PyArg_ParseTuple (args, "sis", &buffer, &y, &message))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("prnt_y");
        PYTHON_RETURN_ERROR;
    }
    
    script_api_printf_y (weechat_python_plugin,
                          python_current_script,
                          script_str2ptr (buffer),
                          y,
                          "%s", message);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_infobar_print: print message to infobar
 */

static PyObject *
weechat_python_api_infobar_print (PyObject *self, PyObject *args)
{
    int delay;
    char *color, *message;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("infobar_print");
        PYTHON_RETURN_ERROR;
    }
    
    delay = 1;
    message = NULL;
    
    if (!PyArg_ParseTuple (args, "iss", &delay, &color, &message))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("infobar_print");
        PYTHON_RETURN_ERROR;
    }
    
    script_api_infobar_printf (weechat_python_plugin,
                               python_current_script,
                               delay, color, "%s", message);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_infobar_remove: remove message(s) from infobar
 */

static PyObject *
weechat_python_api_infobar_remove (PyObject *self, PyObject *args)
{
    int how_many;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("infobar_remove");
        PYTHON_RETURN_ERROR;
    }
    
    how_many = 0;
    
    if (!PyArg_ParseTuple (args, "|i", &how_many))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("infobar_remove");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_infobar_remove (how_many);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_log_print: print message in WeeChat log file
 */

static PyObject *
weechat_python_api_log_print (PyObject *self, PyObject *args)
{
    char *message;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("log_print");
        PYTHON_RETURN_ERROR;
    }
    
    message = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &message))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("log_print");
        PYTHON_RETURN_ERROR;
    }
    
    script_api_log_printf (weechat_python_plugin,
                           python_current_script,
                           "%s", message);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_hook_command_cb: callback for command hooked
 */

int
weechat_python_api_hook_command_cb (void *data, struct t_gui_buffer *buffer,
                                    int argc, char **argv, char **argv_eol)
{
    struct t_script_callback *script_callback;
    char *python_argv[3], empty_arg[1] = { '\0' };
    int *rc, ret;

    /* make C compiler happy */
    (void) argv;
    
    script_callback = (struct t_script_callback *)data;

    python_argv[0] = script_ptr2str (buffer);
    python_argv[1] = (argc > 1) ? argv_eol[1] : empty_arg;
    python_argv[2] = NULL;
    
    rc = (int *) weechat_python_exec (script_callback->script,
                                      WEECHAT_SCRIPT_EXEC_INT,
                                      script_callback->function,
                                      python_argv);
    
    if (!rc)
        ret = WEECHAT_RC_ERROR;
    else
    {
        ret = *rc;
        free (rc);
    }
    if (python_argv[0])
        free (python_argv[0]);
    
    return ret;
}

/*
 * weechat_python_api_hook_command: hook a command
 */

static PyObject *
weechat_python_api_hook_command (PyObject *self, PyObject *args)
{
    char *command, *description, *arguments, *args_description, *completion;
    char *function, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("hook_command");
        PYTHON_RETURN_EMPTY;
    }
    
    command = NULL;
    description = NULL;
    arguments = NULL;
    args_description = NULL;
    completion = NULL;
    function = NULL;
    
    if (!PyArg_ParseTuple (args, "ssssss", &command, &description, &arguments,
                           &args_description, &completion, &function))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("hook_command");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (script_api_hook_command (weechat_python_plugin,
                                                      python_current_script,
                                                      command,
                                                      description,
                                                      arguments,
                                                      args_description,
                                                      completion,
                                                      &weechat_python_api_hook_command_cb,
                                                      function));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_hook_timer_cb: callback for timer hooked
 */

int
weechat_python_api_hook_timer_cb (void *data)
{
    struct t_script_callback *script_callback;
    char *python_argv[1];
    int *rc, ret;
    
    script_callback = (struct t_script_callback *)data;
    
    python_argv[0] = NULL;
    
    rc = (int *) weechat_python_exec (script_callback->script,
                                      WEECHAT_SCRIPT_EXEC_INT,
                                      script_callback->function,
                                      python_argv);
    
    if (!rc)
        ret = WEECHAT_RC_ERROR;
    else
    {
        ret = *rc;
        free (rc);
    }
    
    return ret;
}

/*
 * weechat_python_api_hook_timer: hook a timer
 */

static PyObject *
weechat_python_api_hook_timer (PyObject *self, PyObject *args)
{
    int interval, align_second, max_calls;
    char *function, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("hook_timer");
        PYTHON_RETURN_EMPTY;
    }
    
    interval = 10;
    align_second = 0;
    max_calls = 0;
    function = NULL;
    
    if (!PyArg_ParseTuple (args, "iiis", &interval, &align_second, &max_calls,
                           &function))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("hook_timer");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (script_api_hook_timer (weechat_python_plugin,
                                                    python_current_script,
                                                    interval,
                                                    align_second,
                                                    max_calls,
                                                    &weechat_python_api_hook_timer_cb,
                                                    function));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_hook_fd_cb: callback for fd hooked
 */

int
weechat_python_api_hook_fd_cb (void *data)
{
    struct t_script_callback *script_callback;
    char *python_argv[1];
    int *rc, ret;
    
    script_callback = (struct t_script_callback *)data;
    
    python_argv[0] = NULL;
    
    rc = (int *) weechat_python_exec (script_callback->script,
                                      WEECHAT_SCRIPT_EXEC_INT,
                                      script_callback->function,
                                      python_argv);
    
    if (!rc)
        ret = WEECHAT_RC_ERROR;
    else
    {
        ret = *rc;
        free (rc);
    }
    
    return ret;
}

/*
 * weechat_python_api_hook_fd: hook a fd
 */

static PyObject *
weechat_python_api_hook_fd (PyObject *self, PyObject *args)
{
    int fd, read, write, exception;
    char *function, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("hook_fd");
        PYTHON_RETURN_EMPTY;
    }
    
    fd = 0;
    read = 0;
    write = 0;
    exception = 0;
    function = NULL;
    
    if (!PyArg_ParseTuple (args, "iiiis", &fd, &read, &write, &exception,
                           &function))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("hook_fd");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (script_api_hook_fd (weechat_python_plugin,
                                                 python_current_script,
                                                 fd,
                                                 read,
                                                 write,
                                                 exception,
                                                 &weechat_python_api_hook_fd_cb,
                                                 function));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_hook_print_cb: callback for print hooked
 */

int
weechat_python_api_hook_print_cb (void *data, struct t_gui_buffer *buffer,
                                  time_t date, int tags_count, char **tags,
                                  char *prefix, char *message)
{
    struct t_script_callback *script_callback;
    char *python_argv[6];
    static char timebuffer[64];
    int *rc, ret;
    
    /* make C compiler happy */
    (void) tags_count;
    
    script_callback = (struct t_script_callback *)data;
    
    snprintf (timebuffer, sizeof (timebuffer) - 1, "%ld", date);
    
    python_argv[0] = script_ptr2str (buffer);
    python_argv[1] = timebuffer;
    python_argv[2] = weechat_string_build_with_exploded (tags, ",");
    python_argv[3] = prefix;
    python_argv[4] = message;
    python_argv[5] = NULL;
    
    rc = (int *) weechat_python_exec (script_callback->script,
                                      WEECHAT_SCRIPT_EXEC_INT,
                                      script_callback->function,
                                      python_argv);
    
    if (!rc)
        ret = WEECHAT_RC_ERROR;
    else
    {
        ret = *rc;
        free (rc);
    }
    if (python_argv[0])
        free (python_argv[0]);
    if (python_argv[2])
        free (python_argv[2]);
    
    return ret;
}

/*
 * weechat_python_api_hook_print: hook a print
 */

static PyObject *
weechat_python_api_hook_print (PyObject *self, PyObject *args)
{
    char *buffer, *tags, *message, *function, *result;
    int strip_colors;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("hook_print");
        PYTHON_RETURN_EMPTY;
    }
    
    buffer = NULL;
    tags = NULL;
    message = NULL;
    strip_colors = 0;
    function = NULL;
    
    if (!PyArg_ParseTuple (args, "sssis", &buffer, &tags, &message,
                           &strip_colors, &function))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("hook_print");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str(script_api_hook_print (weechat_python_plugin,
                                                   python_current_script,
                                                   script_str2ptr (buffer),
                                                   tags,
                                                   message,
                                                   strip_colors,
                                                   &weechat_python_api_hook_print_cb,
                                                   function));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_hook_signal_cb: callback for signal hooked
 */

int
weechat_python_api_hook_signal_cb (void *data, char *signal, char *type_data,
                                   void *signal_data)
{
    struct t_script_callback *script_callback;
    char *python_argv[3];
    static char value_str[64], empty_value[1] = { '\0' };
    int *rc, ret, free_needed;
    
    script_callback = (struct t_script_callback *)data;
    
    python_argv[0] = signal;
    free_needed = 0;
    if (strcmp (type_data, WEECHAT_HOOK_SIGNAL_STRING) == 0)
    {
        python_argv[1] = (signal_data) ? (char *)signal_data : empty_value;
    }
    else if (strcmp (type_data, WEECHAT_HOOK_SIGNAL_INT) == 0)
    {
        snprintf (value_str, sizeof (value_str) - 1,
                  "%d", *((int *)signal_data));
        python_argv[1] = value_str;
    }
    else if (strcmp (type_data, WEECHAT_HOOK_SIGNAL_POINTER) == 0)
    {
        python_argv[1] = script_ptr2str (signal_data);
        free_needed = 1;
    }
    else
        python_argv[1] = NULL;
    python_argv[2] = NULL;
    
    rc = (int *) weechat_python_exec (script_callback->script,
                                      WEECHAT_SCRIPT_EXEC_INT,
                                      script_callback->function,
                                      python_argv);
    
    if (!rc)
        ret = WEECHAT_RC_ERROR;
    else
    {
        ret = *rc;
        free (rc);
    }
    if (free_needed && python_argv[1])
        free (python_argv[1]);
    
    return ret;
}

/*
 * weechat_python_api_hook_signal: hook a signal
 */

static PyObject *
weechat_python_api_hook_signal (PyObject *self, PyObject *args)
{
    char *signal, *function, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("hook_signal");
        PYTHON_RETURN_EMPTY;
    }
    
    signal = NULL;
    function = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &signal, &function))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("hook_signal");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (script_api_hook_signal (weechat_python_plugin,
                                                     python_current_script,
                                                     signal,
                                                     &weechat_python_api_hook_signal_cb,
                                                     function));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_hook_signal_send: send a signal
 */

static PyObject *
weechat_python_api_hook_signal_send (PyObject *self, PyObject *args)
{
    char *signal, *type_data, *signal_data, *error;
    int number;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("hook_signal_send");
        PYTHON_RETURN_ERROR;
    }
    
    signal = NULL;
    type_data = NULL;
    signal_data = NULL;
    
    if (!PyArg_ParseTuple (args, "sss", &signal, &type_data, &signal_data))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("hook_signal_send");
        PYTHON_RETURN_ERROR;
    }
    
    if (strcmp (type_data, WEECHAT_HOOK_SIGNAL_STRING) == 0)
    {
        weechat_hook_signal_send (signal, type_data, signal_data);
        PYTHON_RETURN_OK;
    }
    else if (strcmp (type_data, WEECHAT_HOOK_SIGNAL_INT) == 0)
    {
        error = NULL;
        number = (int)strtol (signal_data, &error, 10);
        if (error && !error[0])
        {
            weechat_hook_signal_send (signal, type_data, &number);
        }
        PYTHON_RETURN_OK;
    }
    else if (strcmp (type_data, WEECHAT_HOOK_SIGNAL_POINTER) == 0)
    {
        weechat_hook_signal_send (signal, type_data,
                                  script_str2ptr (signal_data));
        PYTHON_RETURN_OK;
    }
    
    PYTHON_RETURN_ERROR;
}

/*
 * weechat_python_api_hook_config_cb: callback for config option hooked
 */

int
weechat_python_api_hook_config_cb (void *data, char *option, char *value)
{
    struct t_script_callback *script_callback;
    char *python_argv[3];
    int *rc, ret;
    
    script_callback = (struct t_script_callback *)data;
    
    python_argv[0] = option;
    python_argv[1] = value;
    python_argv[2] = NULL;
    
    rc = (int *) weechat_python_exec (script_callback->script,
                                      WEECHAT_SCRIPT_EXEC_INT,
                                      script_callback->function,
                                      python_argv);
    
    if (!rc)
        ret = WEECHAT_RC_ERROR;
    else
    {
        ret = *rc;
        free (rc);
    }
    
    return ret;
}

/*
 * weechat_python_api_hook_config: hook a config option
 */

static PyObject *
weechat_python_api_hook_config (PyObject *self, PyObject *args)
{
    char *option, *function, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("hook_config");
        PYTHON_RETURN_EMPTY;
    }
    
    option = NULL;
    function = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &option, &function))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("hook_config");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str(script_api_hook_config (weechat_python_plugin,
                                                    python_current_script,
                                                    option,
                                                    &weechat_python_api_hook_config_cb,
                                                    function));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_hook_completion_cb: callback for completion hooked
 */

int
weechat_python_api_hook_completion_cb (void *data, char *completion,
                                       struct t_gui_buffer *buffer,
                                       struct t_weelist *list)
{
    struct t_script_callback *script_callback;
    char *python_argv[4];
    int *rc, ret;
    
    script_callback = (struct t_script_callback *)data;
    
    python_argv[0] = completion;
    python_argv[1] = script_ptr2str (buffer);
    python_argv[2] = script_ptr2str (list);
    python_argv[3] = NULL;
    
    rc = (int *) weechat_python_exec (script_callback->script,
                                      WEECHAT_SCRIPT_EXEC_INT,
                                      script_callback->function,
                                      python_argv);
    
    if (!rc)
        ret = WEECHAT_RC_ERROR;
    else
    {
        ret = *rc;
        free (rc);
    }
    if (python_argv[1])
        free (python_argv[1]);
    if (python_argv[2])
        free (python_argv[2]);
    
    return ret;
}

/*
 * weechat_python_api_hook_completion: hook a completion
 */

static PyObject *
weechat_python_api_hook_completion (PyObject *self, PyObject *args)
{
    char *completion, *function, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("hook_completion");
        PYTHON_RETURN_EMPTY;
    }
    
    completion = NULL;
    function = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &completion, &function))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("hook_completion");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str(script_api_hook_completion (weechat_python_plugin,
                                                        python_current_script,
                                                        completion,
                                                        &weechat_python_api_hook_completion_cb,
                                                        function));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_hook_modifier_cb: callback for modifier hooked
 */

char *
weechat_python_api_hook_modifier_cb (void *data, char *modifier,
                                     char *modifier_data, char *string)
{
    struct t_script_callback *script_callback;
    char *python_argv[4];
    
    script_callback = (struct t_script_callback *)data;
    
    python_argv[0] = modifier;
    python_argv[1] = modifier_data;
    python_argv[2] = string;
    python_argv[3] = NULL;
    
    return (char *)weechat_python_exec (script_callback->script,
                                        WEECHAT_SCRIPT_EXEC_STRING,
                                        script_callback->function,
                                        python_argv);
}

/*
 * weechat_python_api_hook_modifier: hook a modifier
 */

static PyObject *
weechat_python_api_hook_modifier (PyObject *self, PyObject *args)
{
    char *modifier, *function, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("hook_modifier");
        PYTHON_RETURN_EMPTY;
    }
    
    modifier = NULL;
    function = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &modifier, &function))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("hook_modifier");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str(script_api_hook_modifier (weechat_python_plugin,
                                                      python_current_script,
                                                      modifier,
                                                      &weechat_python_api_hook_modifier_cb,
                                                      function));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_hook_modifier_exec: execute a modifier hook
 */

static PyObject *
weechat_python_api_hook_modifier_exec (PyObject *self, PyObject *args)
{
    char *modifier, *modifier_data, *string, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("hook_modifier_exec");
        PYTHON_RETURN_EMPTY;
    }
    
    modifier = NULL;
    modifier_data = NULL;
    string = NULL;
    
    if (!PyArg_ParseTuple (args, "sss", &modifier, &modifier_data, &string))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("hook_modifier_exec");
        PYTHON_RETURN_EMPTY;
    }
    
    result = weechat_hook_modifier_exec (modifier, modifier_data, string);
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_unhook: unhook something
 */

static PyObject *
weechat_python_api_unhook (PyObject *self, PyObject *args)
{
    char *hook;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("unhook");
        PYTHON_RETURN_ERROR;
    }
    
    hook = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &hook))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("unhook");
        PYTHON_RETURN_ERROR;
    }
    
    script_api_unhook (weechat_python_plugin,
                       python_current_script,
                       script_str2ptr (hook));
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_unhook_all: unhook all for script
 */

static PyObject *
weechat_python_api_unhook_all (PyObject *self, PyObject *args)
{
    /* make C compiler happy */
    (void) self;
    (void) args;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("unhook_all");
        PYTHON_RETURN_ERROR;
    }
    
    script_api_unhook_all (python_current_script);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_buffer_input_data_cb: callback for input data in a buffer
 */

int
weechat_python_api_buffer_input_data_cb (void *data, struct t_gui_buffer *buffer,
                                         char *input_data)
{
    struct t_script_callback *script_callback;
    char *python_argv[3];
    int *r, ret;
    
    script_callback = (struct t_script_callback *)data;
    
    python_argv[0] = script_ptr2str (buffer);
    python_argv[1] = input_data;
    python_argv[2] = NULL;
    
    r = (int *) weechat_python_exec (script_callback->script,
                                     WEECHAT_SCRIPT_EXEC_INT,
                                     script_callback->function,
                                     python_argv);
    if (!r)
        ret = WEECHAT_RC_ERROR;
    else
    {
        ret = *r;
        free (r);
    }
    if (python_argv[0])
        free (python_argv[0]);
    
    return ret;
}

/*
 * weechat_python_api_buffer_close_cb: callback for buffer closed
 */

int
weechat_python_api_buffer_close_cb (void *data, struct t_gui_buffer *buffer)
{
    struct t_script_callback *script_callback;
    char *python_argv[2];
    int *r, ret;
    
    script_callback = (struct t_script_callback *)data;
    
    python_argv[0] = script_ptr2str (buffer);
    python_argv[1] = NULL;
    
    r = (int *) weechat_python_exec (script_callback->script,
                                     WEECHAT_SCRIPT_EXEC_INT,
                                     script_callback->function,
                                     python_argv);
    if (!r)
        ret = WEECHAT_RC_ERROR;
    else
    {
        ret = *r;
        free (r);
    }
    if (python_argv[0])
        free (python_argv[0]);
    
    return ret;
}

/*
 * weechat_python_api_buffer_new: create a new buffer
 */

static PyObject *
weechat_python_api_buffer_new (PyObject *self, PyObject *args)
{
    char *category, *name, *function_input, *function_close, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("buffer_new");
        PYTHON_RETURN_EMPTY;
    }
    
    category = NULL;
    name = NULL;
    function_input = NULL;
    function_close = NULL;
    
    if (!PyArg_ParseTuple (args, "ssss", &category, &name, &function_input,
                           &function_close))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("buffer_new");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (script_api_buffer_new (weechat_python_plugin,
                                                    python_current_script,
                                                    category,
                                                    name,
                                                    &weechat_python_api_buffer_input_data_cb,
                                                    function_input,
                                                    &weechat_python_api_buffer_close_cb,
                                                    function_close));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_buffer_search: search a buffer
 */

static PyObject *
weechat_python_api_buffer_search (PyObject *self, PyObject *args)
{
    char *category, *name;
    char *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("buffer_search");
        PYTHON_RETURN_EMPTY;
    }
    
    category = NULL;
    name = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &category, &name))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("buffer_search");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_buffer_search (category, name));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_buffer_clear: clear a buffer
 */

static PyObject *
weechat_python_api_buffer_clear (PyObject *self, PyObject *args)
{
    char *buffer;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("buffer_clear");
        PYTHON_RETURN_ERROR;
    }
    
    buffer = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &buffer))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("buffer_clear");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_buffer_clear (script_str2ptr (buffer));
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_buffer_close: close a buffer
 */

static PyObject *
weechat_python_api_buffer_close (PyObject *self, PyObject *args)
{
    char *buffer;
    int switch_to_another;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("buffer_close");
        PYTHON_RETURN_ERROR;
    }
    
    buffer = NULL;
    switch_to_another = 0;
    
    if (!PyArg_ParseTuple (args, "si", &buffer, &switch_to_another))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("buffer_close");
        PYTHON_RETURN_ERROR;
    }
    
    script_api_buffer_close (weechat_python_plugin,
                             python_current_script,
                             script_str2ptr (buffer),
                             switch_to_another);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_buffer_get_string: get a buffer property as string
 */

static PyObject *
weechat_python_api_buffer_get_string (PyObject *self, PyObject *args)
{
    char *buffer, *property, *value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("buffer_get_string");
        PYTHON_RETURN_ERROR;
    }
    
    buffer = NULL;
    property = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &buffer, &property))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("buffer_get_string");
        PYTHON_RETURN_EMPTY;
    }
    
    value = weechat_buffer_get_string (script_str2ptr (buffer), property);
    
    PYTHON_RETURN_STRING(value);
}

/*
 * weechat_python_api_buffer_get_pointer: get a buffer property as pointer
 */

static PyObject *
weechat_python_api_buffer_get_pointer (PyObject *self, PyObject *args)
{
    char *buffer, *property, *value;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("buffer_get_pointer");
        PYTHON_RETURN_ERROR;
    }
    
    buffer = NULL;
    property = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &buffer, &property))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("buffer_get_pointer");
        PYTHON_RETURN_EMPTY;
    }
    
    value = script_ptr2str (weechat_buffer_get_pointer (script_str2ptr (buffer),
                                                        property));
    
    PYTHON_RETURN_STRING_FREE(value);
}

/*
 * weechat_python_api_buffer_set: set a buffer property
 */

static PyObject *
weechat_python_api_buffer_set (PyObject *self, PyObject *args)
{
    char *buffer, *property, *value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("buffer_set");
        PYTHON_RETURN_ERROR;
    }
    
    buffer = NULL;
    property = NULL;
    value = NULL;
    
    if (!PyArg_ParseTuple (args, "sss", &buffer, &property, &value))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("buffer_set");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_buffer_set (script_str2ptr (buffer),
                        property,
                        value);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_nicklist_add_group: add a group in nicklist
 */

static PyObject *
weechat_python_api_nicklist_add_group (PyObject *self, PyObject *args)
{
    char *buffer, *parent_group, *name, *color, *result;
    int visible;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("nicklist_add_group");
        PYTHON_RETURN_EMPTY;
    }
    
    buffer = NULL;
    parent_group = NULL;
    name = NULL;
    color = NULL;
    visible = 0;
    
    if (!PyArg_ParseTuple (args, "ssssi", &buffer, &parent_group, &name,
                           &color, &visible))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("nicklist_add_group");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_nicklist_add_group (script_str2ptr (buffer),
                                                         script_str2ptr (parent_group),
                                                         name,
                                                         color,
                                                         visible));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_nicklist_search_group: search a group in nicklist
 */

static PyObject *
weechat_python_api_nicklist_search_group (PyObject *self, PyObject *args)
{
    char *buffer, *from_group, *name, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("nicklist_search_group");
        PYTHON_RETURN_EMPTY;
    }
    
    buffer = NULL;
    from_group = NULL;
    name = NULL;
    
    if (!PyArg_ParseTuple (args, "sss", &buffer, &from_group, &name))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("nicklist_search_group");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_nicklist_search_group (script_str2ptr (buffer),
                                                            script_str2ptr (from_group),
                                                            name));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_nicklist_add_nick: add a nick in nicklist
 */

static PyObject *
weechat_python_api_nicklist_add_nick (PyObject *self, PyObject *args)
{
    char *buffer, *group, *name, *color, *prefix, *prefix_color, *result;
    char char_prefix;
    int visible;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("nicklist_add_nick");
        PYTHON_RETURN_EMPTY;
    }
    
    buffer = NULL;
    group = NULL;
    name = NULL;
    color = NULL;
    prefix = NULL;
    prefix_color = NULL;
    visible = 0;
    
    if (!PyArg_ParseTuple (args, "ssssssi", &buffer, &group, &name, &color,
                           &prefix, &prefix_color, &visible))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("nicklist_add_nick");
        PYTHON_RETURN_EMPTY;
    }
    
    if (prefix && prefix[0])
        char_prefix = prefix[0];
    else
        char_prefix = ' ';
    
    result = script_ptr2str (weechat_nicklist_add_nick (script_str2ptr (buffer),
                                                        script_str2ptr (group),
                                                        name,
                                                        color,
                                                        char_prefix,
                                                        prefix_color,
                                                        visible));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_nicklist_search_nick: search a nick in nicklist
 */

static PyObject *
weechat_python_api_nicklist_search_nick (PyObject *self, PyObject *args)
{
    char *buffer, *from_group, *name, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("nicklist_search_nick");
        PYTHON_RETURN_EMPTY;
    }
    
    buffer = NULL;
    from_group = NULL;
    name = NULL;
    
    if (!PyArg_ParseTuple (args, "sss", &buffer, &from_group, &name))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("nicklist_search_nick");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_nicklist_search_nick (script_str2ptr (buffer),
                                                           script_str2ptr (from_group),
                                                           name));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_nicklist_remove_group: remove a group from nicklist
 */

static PyObject *
weechat_python_api_nicklist_remove_group (PyObject *self, PyObject *args)
{
    char *buffer, *group;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("nicklist_remove_group");
        PYTHON_RETURN_ERROR;
    }
    
    buffer = NULL;
    group = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &buffer, &group))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("nicklist_remove_group");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_nicklist_remove_group (script_str2ptr (buffer),
                                   script_str2ptr (group));
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_nicklist_remove_nick: remove a nick from nicklist
 */

static PyObject *
weechat_python_api_nicklist_remove_nick (PyObject *self, PyObject *args)
{
    char *buffer, *nick;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("nicklist_remove_nick");
        PYTHON_RETURN_ERROR;
    }
    
    buffer = NULL;
    nick = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &buffer, &nick))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("nicklist_remove_nick");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_nicklist_remove_group (script_str2ptr (buffer),
                                   script_str2ptr (nick));
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_nicklist_remove_all: remove all groups/nicks from nicklist
 */

static PyObject *
weechat_python_api_nicklist_remove_all (PyObject *self, PyObject *args)
{
    char *buffer;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("nicklist_remove_all");
        PYTHON_RETURN_ERROR;
    }
    
    buffer = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &buffer))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("nicklist_remove_all");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_nicklist_remove_all (script_str2ptr (buffer));
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_bar_item_search: search a bar item
 */

static PyObject *
weechat_python_api_bar_item_search (PyObject *self, PyObject *args)
{
    char *name, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("bar_item_search");
        PYTHON_RETURN_EMPTY;
    }
    
    name = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &name))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("bar_item_search");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_bar_item_search (name));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_bar_item_build_cb: callback for building bar item
 */

char *
weechat_python_api_bar_item_build_cb (void *data, struct t_gui_bar_item *item,
                                      struct t_gui_window *window,
                                      int max_width, int max_height)
{
    struct t_script_callback *script_callback;
    char *python_argv[5], *ret;
    static char str_width[32], str_height[32];
    
    script_callback = (struct t_script_callback *)data;

    snprintf (str_width, sizeof (str_width), "%d", max_width);
    snprintf (str_height, sizeof (str_height), "%d", max_height);
    
    python_argv[0] = script_ptr2str (item);
    python_argv[1] = script_ptr2str (window);
    python_argv[2] = str_width;
    python_argv[3] = str_height;
    python_argv[4] = NULL;
    
    ret = (char *)weechat_python_exec (script_callback->script,
                                       WEECHAT_SCRIPT_EXEC_STRING,
                                       script_callback->function,
                                       python_argv);
    
    if (python_argv[0])
        free (python_argv[0]);
    if (python_argv[1])
        free (python_argv[1]);
    
    return ret;
}

/*
 * weechat_python_api_bar_item_new: add a new bar item
 */

static PyObject *
weechat_python_api_bar_item_new (PyObject *self, PyObject *args)
{
    char *name, *function, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("bar_item_new");
        PYTHON_RETURN_EMPTY;
    }
    
    name = NULL;
    function = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &name, &function))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("bar_item_new");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (script_api_bar_item_new (weechat_python_plugin,
                                                      python_current_script,
                                                      name,
                                                      &weechat_python_api_bar_item_build_cb,
                                                      function));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_bar_item_update: update a bar item on screen
 */

static PyObject *
weechat_python_api_bar_item_update (PyObject *self, PyObject *args)
{
    char *name;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("bar_item_update");
        PYTHON_RETURN_ERROR;
    }
    
    name = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &name))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("bar_item_update");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_bar_item_update (name);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_bar_item_remove: remove a bar item
 */

static PyObject *
weechat_python_api_bar_item_remove (PyObject *self, PyObject *args)
{
    char *item;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("bar_item_remove");
        PYTHON_RETURN_ERROR;
    }
    
    item = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &item))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("bar_item_remove");
        PYTHON_RETURN_ERROR;
    }
    
    script_api_bar_item_remove (weechat_python_plugin,
                                python_current_script,
                                script_str2ptr (item));
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_bar_search: search a bar
 */

static PyObject *
weechat_python_api_bar_search (PyObject *self, PyObject *args)
{
    char *name, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("bar_search");
        PYTHON_RETURN_EMPTY;
    }
    
    name = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &name))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("bar_search");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_bar_search (name));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_bar_new: add a new bar
 */

static PyObject *
weechat_python_api_bar_new (PyObject *self, PyObject *args)
{
    char *name, *priority, *type, *conditions, *position, *filling, *size;
    char *size_max, *color_fg, *color_bg, *separator, *items, *result;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("bar_new");
        PYTHON_RETURN_EMPTY;
    }
    
    name = NULL;
    priority = NULL;
    type = NULL;
    conditions = NULL;
    position = NULL;
    filling = NULL;
    size = NULL;
    size_max = NULL;
    color_fg = NULL;
    color_bg = NULL;
    separator = NULL;
    items = NULL;
    
    if (!PyArg_ParseTuple (args, "ssssssssssss", &name, &priority, &conditions,
                           &type, &position, &filling, &size, &size_max,
                           &color_fg, &color_bg, &separator, &items))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("bar_new");
        PYTHON_RETURN_EMPTY;
    }
    
    result = script_ptr2str (weechat_bar_new (name,
                                              priority,
                                              type,
                                              conditions,
                                              position,
                                              filling,
                                              size,
                                              size_max,
                                              color_fg,
                                              color_bg,
                                              separator,
                                              items));
    
    PYTHON_RETURN_STRING_FREE(result);
}

/*
 * weechat_python_api_bar_set: set a bar property
 */

static PyObject *
weechat_python_api_bar_set (PyObject *self, PyObject *args)
{
    char *bar, *property, *value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("bar_set");
        PYTHON_RETURN_ERROR;
    }
    
    bar = NULL;
    property = NULL;
    value = NULL;
    
    if (!PyArg_ParseTuple (args, "sss", &bar, &property, &value))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("bar_set");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_buffer_set (script_str2ptr (bar),
                        property,
                        value);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_bar_update: update a bar on screen
 */

static PyObject *
weechat_python_api_bar_update (PyObject *self, PyObject *args)
{
    char *name;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("bar_item");
        PYTHON_RETURN_ERROR;
    }
    
    name = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &name))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("bar_item");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_bar_update (name);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_bar_remove: remove a bar
 */

static PyObject *
weechat_python_api_bar_remove (PyObject *self, PyObject *args)
{
    char *bar;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("bar_remove");
        PYTHON_RETURN_ERROR;
    }
    
    bar = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &bar))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("bar_remove");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_bar_remove (script_str2ptr (bar));
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_command: send command to server
 */

static PyObject *
weechat_python_api_command (PyObject *self, PyObject *args)
{
    char *buffer, *command;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("command");
        PYTHON_RETURN_ERROR;
    }
    
    buffer = NULL;
    command = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &buffer, &command))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("command");
        PYTHON_RETURN_ERROR;
    }
    
    script_api_command (weechat_python_plugin,
                        python_current_script,
                        script_str2ptr (buffer),
                        command);
    
    PYTHON_RETURN_OK;
}

/*
 * weechat_python_api_info_get: get info about WeeChat
 */

static PyObject *
weechat_python_api_info_get (PyObject *self, PyObject *args)
{
    char *info, *value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("info_get");
        PYTHON_RETURN_EMPTY;
    }
    
    info = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &info))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("info_get");
        PYTHON_RETURN_EMPTY;
    }
    
    value = weechat_info_get (info);
    
    PYTHON_RETURN_STRING(value);
}

/*
 * weechat_python_api_infolist_get: get list with infos
 */

static PyObject *
weechat_python_api_infolist_get (PyObject *self, PyObject *args)
{
    char *name, *pointer, *arguments, *value;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("infolist_get");
        PYTHON_RETURN_EMPTY;
    }
    
    name = NULL;
    pointer = NULL;
    arguments = NULL;
    
    if (!PyArg_ParseTuple (args, "sss", &name, &pointer, &arguments))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("infolist_get");
        PYTHON_RETURN_EMPTY;
    }
    
    value = script_ptr2str (weechat_infolist_get (name,
                                                  script_str2ptr (pointer),
                                                  arguments));
    
    PYTHON_RETURN_STRING_FREE(value);
}

/*
 * weechat_python_api_infolist_next: move item pointer to next item in infolist
 */

static PyObject *
weechat_python_api_infolist_next (PyObject *self, PyObject *args)
{
    char *infolist;
    int value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("infolist_next");
        PYTHON_RETURN_INT(0);
    }
    
    infolist = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &infolist))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("infolist_next");
        PYTHON_RETURN_INT(0);
    }
    
    value = weechat_infolist_next (script_str2ptr (infolist));
    
    PYTHON_RETURN_INT(value);
}

/*
 * weechat_python_api_infolist_prev: move item pointer to previous item in infolist
 */

static PyObject *
weechat_python_api_infolist_prev (PyObject *self, PyObject *args)
{
    char *infolist;
    int value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("infolist_prev");
        PYTHON_RETURN_INT(0);
    }
    
    infolist = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &infolist))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("infolist_prev");
        PYTHON_RETURN_INT(0);
    }
    
    value = weechat_infolist_prev (script_str2ptr (infolist));
    
    PYTHON_RETURN_INT(value);
}

/*
 * weechat_python_api_infolist_fields: get list of fields for current item of infolist
 */

static PyObject *
weechat_python_api_infolist_fields (PyObject *self, PyObject *args)
{
    char *infolist, *value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("infolist_fields");
        PYTHON_RETURN_EMPTY;
    }
    
    infolist = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &infolist))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("infolist_fields");
        PYTHON_RETURN_EMPTY;
    }
    
    value = weechat_infolist_fields (script_str2ptr (infolist));
    
    PYTHON_RETURN_STRING(value);
}

/*
 * weechat_python_api_infolist_integer: get integer value of a variable in infolist
 */

static PyObject *
weechat_python_api_infolist_integer (PyObject *self, PyObject *args)
{
    char *infolist, *variable;
    int value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("infolist_integer");
        PYTHON_RETURN_INT(0);
    }
    
    infolist = NULL;
    variable = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &infolist, &variable))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("infolist_integer");
        PYTHON_RETURN_INT(0);
    }
    
    value = weechat_infolist_integer (script_str2ptr (infolist),
                                      variable);
    
    PYTHON_RETURN_INT(value);
}

/*
 * weechat_python_api_infolist_string: get string value of a variable in infolist
 */

static PyObject *
weechat_python_api_infolist_string (PyObject *self, PyObject *args)
{
    char *infolist, *variable, *value;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("infolist_string");
        PYTHON_RETURN_EMPTY;
    }
    
    infolist = NULL;
    variable = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &infolist, &variable))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("infolist_string");
        PYTHON_RETURN_EMPTY;
    }
    
    value = weechat_infolist_string (script_str2ptr (infolist),
                                     variable);
    
    PYTHON_RETURN_STRING(value);
}

/*
 * weechat_python_api_infolist_pointer: get pointer value of a variable in infolist
 */

static PyObject *
weechat_python_api_infolist_pointer (PyObject *self, PyObject *args)
{
    char *infolist, *variable, *value;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("infolist_pointer");
        PYTHON_RETURN_EMPTY;
    }
    
    infolist = NULL;
    variable = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &infolist, &variable))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("infolist_pointer");
        PYTHON_RETURN_EMPTY;
    }
    
    value = script_ptr2str (weechat_infolist_string (script_str2ptr (infolist),
                                                     variable));
    
    PYTHON_RETURN_STRING_FREE(value);
}

/*
 * weechat_python_api_infolist_time: get time value of a variable in infolist
 */

static PyObject *
weechat_python_api_infolist_time (PyObject *self, PyObject *args)
{
    char *infolist, *variable, timebuffer[64], *value;
    time_t time;
    PyObject *object;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("infolist_time");
        PYTHON_RETURN_EMPTY;
    }
    
    infolist = NULL;
    variable = NULL;
    
    if (!PyArg_ParseTuple (args, "ss", &infolist, &variable))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("infolist_time");
        PYTHON_RETURN_EMPTY;
    }
    
    time = weechat_infolist_time (script_str2ptr (infolist),
                                  variable);
    strftime (timebuffer, sizeof (timebuffer), "%F %T", localtime (&time));
    value = strdup (timebuffer);
    
    PYTHON_RETURN_STRING_FREE(value);
}

/*
 * weechat_python_api_infolist_free: free infolist
 */

static PyObject *
weechat_python_api_infolist_free (PyObject *self, PyObject *args)
{
    char *infolist;
    
    /* make C compiler happy */
    (void) self;
    
    if (!python_current_script)
    {
        WEECHAT_SCRIPT_MSG_NOT_INITIALIZED("infolist_free");
        PYTHON_RETURN_ERROR;
    }
    
    infolist = NULL;
    
    if (!PyArg_ParseTuple (args, "s", &infolist))
    {
        WEECHAT_SCRIPT_MSG_WRONG_ARGUMENTS("infolist_free");
        PYTHON_RETURN_ERROR;
    }
    
    weechat_infolist_free (script_str2ptr (infolist));
    
    PYTHON_RETURN_OK;
}

/*
 * Python subroutines
 */

PyMethodDef weechat_python_funcs[] =
{
    { "register", &weechat_python_api_register, METH_VARARGS, "" },
    { "charset_set", &weechat_python_api_charset_set, METH_VARARGS, "" },
    { "iconv_to_internal", &weechat_python_api_iconv_to_internal, METH_VARARGS, "" },
    { "iconv_from_internal", &weechat_python_api_iconv_from_internal, METH_VARARGS, "" },
    { "gettext", &weechat_python_api_gettext, METH_VARARGS, "" },
    { "ngettext", &weechat_python_api_ngettext, METH_VARARGS, "" },
    { "mkdir_home", &weechat_python_api_mkdir_home, METH_VARARGS, "" },
    { "mkdir", &weechat_python_api_mkdir, METH_VARARGS, "" },
    { "list_new", &weechat_python_api_list_new, METH_VARARGS, "" },
    { "list_add", &weechat_python_api_list_add, METH_VARARGS, "" },
    { "list_search", &weechat_python_api_list_search, METH_VARARGS, "" },
    { "list_casesearch", &weechat_python_api_list_casesearch, METH_VARARGS, "" },
    { "list_get", &weechat_python_api_list_get, METH_VARARGS, "" },
    { "list_set", &weechat_python_api_list_set, METH_VARARGS, "" },
    { "list_next", &weechat_python_api_list_next, METH_VARARGS, "" },
    { "list_prev", &weechat_python_api_list_prev, METH_VARARGS, "" },
    { "list_string", &weechat_python_api_list_string, METH_VARARGS, "" },
    { "list_size", &weechat_python_api_list_size, METH_VARARGS, "" },
    { "list_remove", &weechat_python_api_list_remove, METH_VARARGS, "" },
    { "list_remove_all", &weechat_python_api_list_remove_all, METH_VARARGS, "" },
    { "list_free", &weechat_python_api_list_free, METH_VARARGS, "" },
    { "config_new", &weechat_python_api_config_new, METH_VARARGS, "" },
    { "config_new_section", &weechat_python_api_config_new_section, METH_VARARGS, "" },
    { "config_search_section", &weechat_python_api_config_search_section, METH_VARARGS, "" },
    { "config_new_option", &weechat_python_api_config_new_option, METH_VARARGS, "" },
    { "config_search_option", &weechat_python_api_config_search_option, METH_VARARGS, "" },
    { "config_string_to_boolean", &weechat_python_api_config_string_to_boolean, METH_VARARGS, "" },
    { "config_option_reset", &weechat_python_api_config_option_reset, METH_VARARGS, "" },
    { "config_option_set", &weechat_python_api_config_option_set, METH_VARARGS, "" },
    { "config_option_rename", &weechat_python_api_config_option_rename, METH_VARARGS, "" },
    { "config_boolean", &weechat_python_api_config_boolean, METH_VARARGS, "" },
    { "config_integer", &weechat_python_api_config_integer, METH_VARARGS, "" },
    { "config_string", &weechat_python_api_config_string, METH_VARARGS, "" },
    { "config_color", &weechat_python_api_config_color, METH_VARARGS, "" },
    { "config_write_line", &weechat_python_api_config_write_line, METH_VARARGS, "" },
    { "config_write", &weechat_python_api_config_write, METH_VARARGS, "" },
    { "config_read", &weechat_python_api_config_read, METH_VARARGS, "" },
    { "config_reload", &weechat_python_api_config_reload, METH_VARARGS, "" },
    { "config_free", &weechat_python_api_config_free, METH_VARARGS, "" },
    { "config_get", &weechat_python_api_config_get, METH_VARARGS, "" },
    { "config_get_plugin", &weechat_python_api_config_get_plugin, METH_VARARGS, "" },
    { "config_set_plugin", &weechat_python_api_config_set_plugin, METH_VARARGS, "" },
    { "prefix", &weechat_python_api_prefix, METH_VARARGS, "" },
    { "color", &weechat_python_api_color, METH_VARARGS, "" },
    { "prnt", &weechat_python_api_prnt, METH_VARARGS, "" },
    { "prnt_date_tags", &weechat_python_api_prnt_date_tags, METH_VARARGS, "" },
    { "prnt_y", &weechat_python_api_prnt_y, METH_VARARGS, "" },
    { "infobar_print", &weechat_python_api_infobar_print, METH_VARARGS, "" },
    { "infobar_remove", &weechat_python_api_infobar_remove, METH_VARARGS, "" },
    { "log_print", &weechat_python_api_log_print, METH_VARARGS, "" },
    { "hook_command", &weechat_python_api_hook_command, METH_VARARGS, "" },
    { "hook_timer", &weechat_python_api_hook_timer, METH_VARARGS, "" },
    { "hook_fd", &weechat_python_api_hook_fd, METH_VARARGS, "" },
    { "hook_print", &weechat_python_api_hook_print, METH_VARARGS, "" },
    { "hook_signal", &weechat_python_api_hook_signal, METH_VARARGS, "" },
    { "hook_signal_send", &weechat_python_api_hook_signal_send, METH_VARARGS, "" },
    { "hook_config", &weechat_python_api_hook_config, METH_VARARGS, "" },
    { "hook_completion", &weechat_python_api_hook_completion, METH_VARARGS, "" },
    { "hook_modifier", &weechat_python_api_hook_modifier, METH_VARARGS, "" },
    { "hook_modifier_exec", &weechat_python_api_hook_modifier_exec, METH_VARARGS, "" },
    { "unhook", &weechat_python_api_unhook, METH_VARARGS, "" },
    { "unhook_all", &weechat_python_api_unhook_all, METH_VARARGS, "" },
    { "buffer_new", &weechat_python_api_buffer_new, METH_VARARGS, "" },
    { "buffer_search", &weechat_python_api_buffer_search, METH_VARARGS, "" },
    { "buffer_clear", &weechat_python_api_buffer_clear, METH_VARARGS, "" },
    { "buffer_close", &weechat_python_api_buffer_close, METH_VARARGS, "" },
    { "buffer_get_string", &weechat_python_api_buffer_get_string, METH_VARARGS, "" },
    { "buffer_get_pointer", &weechat_python_api_buffer_get_pointer, METH_VARARGS, "" },
    { "buffer_set", &weechat_python_api_buffer_set, METH_VARARGS, "" },
    { "nicklist_add_group", &weechat_python_api_nicklist_add_group, METH_VARARGS, "" },
    { "nicklist_search_group", &weechat_python_api_nicklist_search_group, METH_VARARGS, "" },
    { "nicklist_add_nick", &weechat_python_api_nicklist_add_nick, METH_VARARGS, "" },
    { "nicklist_search_nick", &weechat_python_api_nicklist_search_nick, METH_VARARGS, "" },
    { "nicklist_remove_group", &weechat_python_api_nicklist_remove_group, METH_VARARGS, "" },
    { "nicklist_remove_nick", &weechat_python_api_nicklist_remove_nick, METH_VARARGS, "" },
    { "nicklist_remove_all", &weechat_python_api_nicklist_remove_all, METH_VARARGS, "" },
    { "bar_item_search", &weechat_python_api_bar_item_search, METH_VARARGS, "" },
    { "bar_item_new", &weechat_python_api_bar_item_new, METH_VARARGS, "" },
    { "bar_item_update", &weechat_python_api_bar_item_update, METH_VARARGS, "" },
    { "bar_item_remove", &weechat_python_api_bar_item_remove, METH_VARARGS, "" },
    { "bar_search", &weechat_python_api_bar_search, METH_VARARGS, "" },
    { "bar_new", &weechat_python_api_bar_new, METH_VARARGS, "" },
    { "bar_set", &weechat_python_api_bar_set, METH_VARARGS, "" },
    { "bar_update", &weechat_python_api_bar_update, METH_VARARGS, "" },
    { "bar_remove", &weechat_python_api_bar_remove, METH_VARARGS, "" },
    { "command", &weechat_python_api_command, METH_VARARGS, "" },
    { "info_get", &weechat_python_api_info_get, METH_VARARGS, "" },
    { "infolist_get", &weechat_python_api_infolist_get, METH_VARARGS, "" },
    { "infolist_next", &weechat_python_api_infolist_next, METH_VARARGS, "" },
    { "infolist_prev", &weechat_python_api_infolist_prev, METH_VARARGS, "" },
    { "infolist_fields", &weechat_python_api_infolist_fields, METH_VARARGS, "" },
    { "infolist_integer", &weechat_python_api_infolist_integer, METH_VARARGS, "" },
    { "infolist_string", &weechat_python_api_infolist_string, METH_VARARGS, "" },
    { "infolist_pointer", &weechat_python_api_infolist_pointer, METH_VARARGS, "" },
    { "infolist_time", &weechat_python_api_infolist_time, METH_VARARGS, "" },
    { "infolist_free", &weechat_python_api_infolist_free, METH_VARARGS, "" },
    { NULL, NULL, 0, NULL }
};
