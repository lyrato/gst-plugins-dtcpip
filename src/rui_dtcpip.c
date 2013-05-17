// COPYRIGHT_BEGIN
//  DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
//
//  Copyright (C) 2013 Cable Television Laboratories, Inc.
//
//  This program is free software; you can redistribute it and/or modify
//  it under the terms of the GNU General Public License as published by
//  the Free Software Foundation, version 2. This program is distributed
//  in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
//  even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
//  PURPOSE. See the GNU General Public License for more details.
//
//  You should have received a copy of the GNU General Public License along
//  with this program.  If not, see  <http://www.gnu.org/licenses/>.
//
//  Please contact CableLabs if you need additional information or
//  have any questions.
//
//      CableLabs
//      858 Coal Creek Cir
//      Louisville, CO 80027-9750
//      303 661-9100
//      oc-mail@cablelabs.com
//
//  If you or the company you represent has a separate agreement with CableLabs
//  concerning the use of this code, your rights and obligations with respect
//  to this code shall be as set forth therein. No license is granted hereunder
//  for any other purpose.
// COPYRIGHT_END


#include <glib.h>
#include <string.h>

#include "rui_dtcpip.h"
#include "rui_dll.h"

struct dtcpip_untyped_function_entry_s
{
    char *name;
    void (*func)();
};

typedef struct dtcpip_untyped_function_entry_s dtcpip_untyped_function_entry_t;

#define DTCPIP_STUB(ret,func,...) \
    static ret func ## _stub(__VA_ARGS__)

DTCPIP_STUB(int, dtcpip_cmn_init, const char* storage_path);
DTCPIP_STUB(void, dtcpip_cmn_get_version, char* string, size_t length);

DTCPIP_STUB(int, dtcpip_src_init, unsigned short dtcp_port);
DTCPIP_STUB(int, dtcpip_src_open, int* session_handle, int is_audio_only);
DTCPIP_STUB(int, dtcpip_src_alloc_encrypt, int session_handle,
                 unsigned char cci,
                 char* cleartext_data, size_t cleartext_size,
                 char** encrypted_data, size_t* encrypted_size);
DTCPIP_STUB(int, dtcpip_src_free, char* encrypted_data);
DTCPIP_STUB(int, dtcpip_src_close,int session_handle);

DTCPIP_STUB(int, dtcpip_snk_init, void);
DTCPIP_STUB(int, dtcpip_snk_open,
                 char* ip_addr, unsigned short ip_port,
                 int *session_handle);
DTCPIP_STUB(int, dtcpip_snk_alloc_decrypt, int session_handle,
                 char* encrypted_data, size_t encrypted_size,
                 char** cleartext_data, size_t* cleartext_size);
DTCPIP_STUB(int, dtcpip_snk_free, char* cleartext_data);
DTCPIP_STUB(int, dtcpip_snk_close, int session_handle);



#define DTCPIP_INIT(func) \
    { #func, (void(*)()) func ## _stub }
 
static dtcpip_untyped_function_entry_t untyped_functions[] =
{
    DTCPIP_INIT(dtcpip_cmn_init),
    DTCPIP_INIT(dtcpip_cmn_get_version),
    DTCPIP_INIT(dtcpip_src_init),
    DTCPIP_INIT(dtcpip_src_open),
    DTCPIP_INIT(dtcpip_src_alloc_encrypt),
    DTCPIP_INIT(dtcpip_src_free),
    DTCPIP_INIT(dtcpip_src_close),

    DTCPIP_INIT(dtcpip_snk_init),
    DTCPIP_INIT(dtcpip_snk_open),
    DTCPIP_INIT(dtcpip_snk_alloc_decrypt),
    DTCPIP_INIT(dtcpip_snk_free),
    DTCPIP_INIT(dtcpip_snk_close)
};

dtcpip_typed_functions_t* g_dtcpip_ftable = NULL;

static rui_Dlmod dtcp_dll = (rui_Dlmod) 0;

DTCPIP_STUB(int, dtcpip_cmn_init, const char* storage_path)
{
    g_debug("%s NOT IMPLEMENTED\n", __FUNCTION__);

    return TRUE;
}

DTCPIP_STUB(void, dtcpip_cmn_get_version, char* string, size_t length)
{
    g_debug("%s NOT IMPLEMENTED\n", __FUNCTION__);
}

DTCPIP_STUB(int, dtcpip_src_init, unsigned short dtcp_port)
{
    g_debug("%s NOT IMPLEMENTED\n", __FUNCTION__);

    return TRUE;
}

DTCPIP_STUB(int, dtcpip_src_open, int* session_handle, int is_audio_only)
{
    g_debug("%s NOT IMPLEMENTED\n", __FUNCTION__);

    return -1;
}

DTCPIP_STUB(int, dtcpip_src_alloc_encrypt, int session_handle,
                 unsigned char cci,
                 char* cleartext_data, size_t cleartext_size,
                 char** encrypted_data, size_t* encrypted_size)
{
    return -1;
}

DTCPIP_STUB(int, dtcpip_src_free, char* encrypted_data)
{
    return -1;
}

DTCPIP_STUB(int, dtcpip_src_close,int session_handle)
{
    return -1;
}

DTCPIP_STUB(int, dtcpip_snk_init, void)
{
    g_debug("%s NOT IMPLEMENTED\n", __FUNCTION__);

    return TRUE;
}

DTCPIP_STUB(int, dtcpip_snk_open,
                 char* ip_addr, unsigned short ip_port,
                 int *session_handle)
{
    g_debug("%s NOT IMPLEMENTED\n", __FUNCTION__);

    return -1;
}

DTCPIP_STUB(int, dtcpip_snk_alloc_decrypt, int session_handle,
                 char* encrypted_data, size_t encrypted_size,
                 char** cleartext_data, size_t* cleartext_size)
{
    return -1;
}

DTCPIP_STUB(int, dtcpip_snk_free, char* cleartext_data)
{
    return -1;
}

DTCPIP_STUB(int, dtcpip_snk_close, int session_handle)
{
    g_debug("%s NOT IMPLEMENTED\n", __FUNCTION__);

    return -1;
}


/**
 * Initializes the DTCP/IP library. If the library cannot be
 * located or does not have all the required functions, DTCP
 * encryption will be disabled and local stub functions will
 * be used instead.
 */
gboolean rui_dtcpip_init()
{
    g_debug("%s - called\n", __FUNCTION__);

    gboolean ret_code = FALSE;
    const gchar *dll_path = NULL;

    g_dtcpip_ftable = (dtcpip_typed_functions_t*) &untyped_functions;

    dll_path = getenv(RUIH_GST_DTCP_DLL_ENV);
    if (dll_path == NULL)
    {
        g_error("%s - dtcp dll env var %s is NULL.\n", __FUNCTION__, RUIH_GST_DTCP_DLL_ENV);
        return FALSE;
    }
    else
    {
        g_message("%s - opening dll using path: %s\n", __FUNCTION__, dll_path);
        ret_code = rui_dlmodOpen(dll_path, &dtcp_dll);
        if (ret_code == TRUE)
        {
            gint i = 0;
            gint num_funcs = -1;

            g_message("%s - successfully loaded DTCP/IP library from \"%s\".\n",
                    __FUNCTION__, dll_path);

            // First check that all functions defined in the structure can
            // be located in the supplied DLL.Cleaned up tab/spaces in all files
            num_funcs = sizeof(untyped_functions) / sizeof(dtcpip_untyped_function_entry_t);
            for (i = 0; i < num_funcs; i++)
            {
                void (*func)() = NULL;
                ret_code = rui_dlmodGetSymbol(dtcp_dll, untyped_functions[i].name, (void **) &func);
                if (ret_code == TRUE)
                {
                    g_debug("%s - successfully located function \"%s\".\n",
                            __FUNCTION__, untyped_functions[i].name);
                }
                else
                {
                    g_error("%s - unable to locate function \"%s\".\n",
                            __FUNCTION__, untyped_functions[i].name);
                    break;
                }
            }

            // If all symbols can be located, go ahead and re-assign all of the function pointers.
            if (ret_code == TRUE)
            {
                for (i = 0; i < num_funcs; i++)
                {
                    ret_code = rui_dlmodGetSymbol(dtcp_dll, untyped_functions[i].name,
                            (void **) &untyped_functions[i].func);
                    if (ret_code != TRUE)
                    {
                        g_error("%s - unable to get symbol \"%s\".\n",
                               __FUNCTION__, untyped_functions[i].name);
                        return FALSE;
                    }
                }
            }
        }
        else
        {
            g_error("%s - unable to load DTCP/IP library from \"%s\": Shared Library error %d.\n",
                    __FUNCTION__, dll_path, ret_code);
            return FALSE;
        }
    }

    if (ret_code != TRUE)
    {
        g_warning("%s - DTCP/IP disabled.\n", __FUNCTION__);
        return FALSE;
    }

    gpointer dtcp_storage;
    gsize storage_len = 0;

    const gchar* dtcp_storage_env = getenv(RUIH_GST_DTCP_KEY_STORAGE_ENV);
    if (dtcp_storage_env == NULL)
    {
        g_message("%s - %s not defined.\n", __FUNCTION__, RUIH_GST_DTCP_KEY_STORAGE_ENV);
        const gchar *last_slash = strrchr(dll_path, '/');
        if (last_slash != NULL)
        {
            storage_len = (int) (last_slash - dll_path);
            dtcp_storage = g_malloc0(storage_len +1);
            memcpy(dtcp_storage, dll_path, storage_len);
            ((gchar*)dtcp_storage)[storage_len] = '\0';
        }
        else
        {
            storage_len = 1;
            dtcp_storage = g_malloc0(storage_len +1);
            ((gchar*)dtcp_storage)[0] = '.';
            ((gchar*)dtcp_storage)[1] = '\0';
        }
    }
    else
    {
        storage_len = strlen(dtcp_storage_env);
        dtcp_storage = g_malloc0(storage_len +1);
        memcpy(dtcp_storage, dtcp_storage_env, storage_len);
        ((gchar*)dtcp_storage)[storage_len] = '\0';
    }

    gint result = 0;
    g_message("%s - using \"%s\" for DTCP/IP library storage.\n",
            __FUNCTION__, (gchar*)dtcp_storage);
    result = g_dtcpip_ftable->dtcpip_cmn_init(dtcp_storage);
    if (result != 0)
    {
        g_error("%s - dtcpip_cmn_init() failed with %d.\n",
                __FUNCTION__, result);
        g_free(dtcp_storage);
        return FALSE;
    }

    g_free(dtcp_storage);

    g_message("%s - DTCP/IP enabled.\n", __FUNCTION__);

    return TRUE;
}
