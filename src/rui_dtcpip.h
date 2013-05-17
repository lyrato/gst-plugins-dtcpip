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
//  #include <gst/gst.h>

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

#ifndef _RUI_DTCPIP_H_
#define _RUI_DTCPIP_H_

#include <stdlib.h> // size_t
#include <gst/gst.h>

#define DTCPIP_INVALID_SESSION_HANDLE (-1)
#define DTCPIP_IS_SESSION_VALID(h) ((h)>0)

#define DTCPIP_SUCCESS (0)
// dtcpip (CableLabs) error code range -1 through -4
// see Cablelabs dtcpip library functions for more detail

// DTCP (Intel) error code range is below -100
#define IS_DTCPIP_SUCCESS(ret_val) ((ret_val)>=0)
#define IS_DTCPIP_FAILURE(ret_val) ((ret_val)<0)

#define RUIH_GST_DTCP_KEY_STORAGE_ENV "RUIH_GST_DTCP_KEY_STORAGE"
#define RUIH_GST_DTCP_DLL_ENV "RUIH_GST_DTCP_DLL"

#define DTCPIP_DECL(ret,func,...) \
    char* func ## _name; \
    ret (*func)(__VA_ARGS__);

typedef struct
{
    DTCPIP_DECL(int, dtcpip_cmn_init, const char* storage_path)
    DTCPIP_DECL(void, dtcpip_cmn_get_version, char* string, size_t length)

    DTCPIP_DECL(int, dtcpip_src_init, unsigned short dtcp_port)
    DTCPIP_DECL(int, dtcpip_src_open, int* session_handle, int is_audio_only)
    DTCPIP_DECL(int, dtcpip_src_alloc_encrypt, int session_handle,
                     unsigned char cci,
                     char* cleartext_data, size_t cleartext_size,
                     char** encrypted_data, size_t* encrypted_size)
    DTCPIP_DECL(int, dtcpip_src_free, char* encrypted_data)
    DTCPIP_DECL(int, dtcpip_src_close,int session_handle)

    DTCPIP_DECL(int, dtcpip_snk_init, void)
    DTCPIP_DECL(int, dtcpip_snk_open,
                     char* ip_addr, unsigned short ip_port,
                     int *session_handle)
    DTCPIP_DECL(int, dtcpip_snk_alloc_decrypt, int session_handle,
                     char* encrypted_data, size_t encrypted_size,
                     char** cleartext_data, size_t* cleartext_size)
    DTCPIP_DECL(int, dtcpip_snk_free, char* cleartext_data)
    DTCPIP_DECL(int, dtcpip_snk_close, int session_handle)
}
dtcpip_typed_functions_t;

extern dtcpip_typed_functions_t* g_dtcpip_ftable;

gboolean rui_dtcpip_init();

#endif // _RUI_DTCPIP_H_
