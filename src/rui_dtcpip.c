// COPYRIGHT_BEGIN
//  DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
//
//  Copyright (C) 2008-2011, Cable Television Laboratories, Inc.
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

#include <stdio.h>
#include <string.h> // memcpy, strrchr, strlen

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
    //MPEOS_LOG(MPE_LOG_DEBUG, MPE_MOD_HN, "%s NOT IMPLEMENTED\n", __FUNCTION__);

    // need to return success so that hn init suceeds
    return TRUE;
}

DTCPIP_STUB(void, dtcpip_cmn_get_version, char* string, size_t length)
{
    snprintf(string, length, "DTCP NOT IMPLEMENTED");
}

DTCPIP_STUB(int, dtcpip_src_init, unsigned short dtcp_port)
{
    //MPEOS_LOG(MPE_LOG_DEBUG, MPE_MOD_HN, "%s NOT IMPLEMENTED\n", __FUNCTION__);

    // need to return success so that hn init suceeds
    return TRUE;
}

DTCPIP_STUB(int, dtcpip_src_open, int* session_handle, int is_audio_only)
{
    //MPEOS_LOG(MPE_LOG_DEBUG, MPE_MOD_HN, "%s NOT IMPLEMENTED\n", __FUNCTION__);

    return -1;
}

DTCPIP_STUB(int, dtcpip_src_alloc_encrypt, int session_handle,
                 unsigned char cci,
                 char* cleartext_data, size_t cleartext_size,
                 char** encrypted_data, size_t* encrypted_size)
{
    //MPEOS_LOG(MPE_LOG_DEBUG, MPE_MOD_HN, "%s NOT IMPLEMENTED\n", __FUNCTION__);

    return -1;
}

DTCPIP_STUB(int, dtcpip_src_free, char* encrypted_data)
{
    //MPEOS_LOG(MPE_LOG_DEBUG, MPE_MOD_HN, "%s NOT IMPLEMENTED\n", __FUNCTION__);

    return -1;
}

DTCPIP_STUB(int, dtcpip_src_close,int session_handle)
{
    //MPEOS_LOG(MPE_LOG_DEBUG, MPE_MOD_HN, "%s NOT IMPLEMENTED\n", __FUNCTION__);

    return -1;
}

DTCPIP_STUB(int, dtcpip_snk_init, void)
{
    //MPEOS_LOG(MPE_LOG_DEBUG, MPE_MOD_HN, "%s NOT IMPLEMENTED\n", __FUNCTION__);

    // need to return success so that hn init suceeds
    return TRUE;
}

DTCPIP_STUB(int, dtcpip_snk_open,
                 char* ip_addr, unsigned short ip_port,
                 int *session_handle)
{
    //MPEOS_LOG(MPE_LOG_DEBUG, MPE_MOD_HN, "%s NOT IMPLEMENTED\n", __FUNCTION__);

    return -1;
}

DTCPIP_STUB(int, dtcpip_snk_alloc_decrypt, int session_handle,
                 char* encrypted_data, size_t encrypted_size,
                 char** cleartext_data, size_t* cleartext_size)
{
    //MPEOS_LOG(MPE_LOG_DEBUG, MPE_MOD_HN, "%s NOT IMPLEMENTED\n", __FUNCTION__);

    return -1;
}

DTCPIP_STUB(int, dtcpip_snk_free, char* cleartext_data)
{
    //MPEOS_LOG(MPE_LOG_DEBUG, MPE_MOD_HN, "%s NOT IMPLEMENTED\n", __FUNCTION__);
    
    return -1;
}

DTCPIP_STUB(int, dtcpip_snk_close, int session_handle)
{
    //MPEOS_LOG(MPE_LOG_DEBUG, MPE_MOD_HN, "%s NOT IMPLEMENTED\n", __FUNCTION__);

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
	printf("rui_dtcpip_init() called\n");

    gboolean ret_code = FALSE;
    const char *dll_path = NULL;

    g_dtcpip_ftable = (dtcpip_typed_functions_t*) &untyped_functions;

    dll_path = getenv(RUIH_GST_DTCP_DLL_ENV);
    if (dll_path == NULL)
    {
        g_printerr("%s - dtcp dll env var %s is NULL.\n", __FUNCTION__, RUIH_GST_DTCP_DLL_ENV);
        return FALSE;
    }
    else
    {
    	printf("%s - opening dll using path: %s\n", __FUNCTION__, dll_path);
        ret_code = rui_dlmodOpen(dll_path, &dtcp_dll);
        if (ret_code == TRUE)
        {
            int          i = 0;
            int num_funcs = -1;

            printf("%s - successfully loaded DTCP/IP library from \"%s\".\n",
            		__FUNCTION__, dll_path);

            // First check that all functions defined in the structure can
            // be located in the supplied DLL.
            num_funcs = sizeof(untyped_functions) / sizeof(dtcpip_untyped_function_entry_t);
            for (i = 0; i < num_funcs; i++)
            {
                void (*func)() = NULL;
                ret_code = rui_dlmodGetSymbol(dtcp_dll, untyped_functions[i].name, (void **) &func);
                if (ret_code == TRUE)
                {
                    printf("%s - successfully located function \"%s\".\n",
                        __FUNCTION__, untyped_functions[i].name);
                }
                else
                {
                    g_printerr("%s - unable to locate function \"%s\".\n",
                    		__FUNCTION__, untyped_functions[i].name);
                    break;
                }
            }

            // If all symbols can be located, go ahead and re-assign all of the function pointers.
            if (ret_code == TRUE)
            {
                for (i = 0; i < num_funcs; i++)
                {
                    ret_code = rui_dlmodGetSymbol(dtcp_dll, untyped_functions[i].name, (void **) &untyped_functions[i].func);
                    if (ret_code != TRUE)
                    {
                        g_printerr("%s - unable to get symbol \"%s\".\n",
                               __FUNCTION__, untyped_functions[i].name);
                        return FALSE;
                    }
                }
            }
        }
        else
        {
            g_printerr("%s - unable to load DTCP/IP library from \"%s\": MPEOS DLL error %d.\n",
            		__FUNCTION__, dll_path, ret_code);
            return FALSE;
        }
    }

    if (ret_code != TRUE)
    {
        g_printerr("%s - DTCP/IP disabled.\n", __FUNCTION__);
        return FALSE;
    }

    char dtcp_storage[256];
    int    storage_len = 0;

    const char* dtcp_storage_env = getenv(RUIH_GST_DTCP_KEY_STORAGE_ENV);
    if (dtcp_storage_env == NULL)
    {
        printf("%s - %s not defined.\n", __FUNCTION__, RUIH_GST_DTCP_KEY_STORAGE_ENV);
        const char *last_slash = strrchr(dll_path, '/');
        if (last_slash != NULL)
        {
            storage_len = (int) (last_slash - dll_path);
            //ret_code = mpeos_memAllocP(MPE_MEM_HN, storage_len + 1, (void **) &dtcp_storage);
            //if (ret_code != MPE_SUCCESS)
            //{
            //    MPEOS_LOG(MPE_LOG_ERROR, MPE_MOD_HN, "%s - failed to allocate memory to copy \"%s\".\n",
            //            __FUNCTION__, DLL_PATH_ENV);
            //    return MPE_HN_ERR_OS_FAILURE;
            //}
            //else
            //{
                memcpy(dtcp_storage, dll_path, storage_len);
                dtcp_storage[storage_len] = '\0';
            //}
        }
        else
        {
            storage_len = 1;
            /*
            ret_code = mpeos_memAllocP(MPE_MEM_HN, 2, (void **) &dtcp_storage);
            if (ret_code != MPE_SUCCESS)
            {
                MPEOS_LOG(MPE_LOG_ERROR, MPE_MOD_HN, "%s - failed to allocate memory to copy and terminate \"%s\".\n",
                        __FUNCTION__, DLL_PATH_ENV);
                return MPE_HN_ERR_OS_FAILURE;
            }
            else
            {
            */
                dtcp_storage[0] = '.';
                dtcp_storage[1] = '\0';
            //}
        }
    }
    else
    {
        storage_len = strlen(dtcp_storage_env);
        /*
        ret_code = mpeos_memAllocP(MPE_MEM_HN, storage_len + 1, (void **) &dtcp_storage);
        if (ret_code != MPE_SUCCESS)
        {
            MPEOS_LOG(MPE_LOG_ERROR, MPE_MOD_HN, "%s - failed to allocate memory to copy env \"%s\".\n",
                    __FUNCTION__, DTCP_STORAGE_ENV);
            return MPE_HN_ERR_OS_FAILURE;
        }
        else
        {
        */
            memcpy(dtcp_storage, dtcp_storage_env, storage_len);
            dtcp_storage[storage_len] = '\0';
        //}
    }

    int result = 0;
    printf("%s - using \"%s\" for DTCP/IP library storage.\n",
            __FUNCTION__, dtcp_storage);
    result = g_dtcpip_ftable->dtcpip_cmn_init(dtcp_storage);
    if (result != 0)
    {
        g_printerr("%s - dtcpip_cmn_init() failed with %d.\n",
                __FUNCTION__, result);
        return FALSE;
    }

    //mpeos_memFreeP(MPE_MEM_HN, (void *) dtcp_storage);

    printf("%s - DTCP/IP enabled.\n", __FUNCTION__);

    return TRUE;
}
