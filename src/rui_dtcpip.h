/* Copyright (C) 2013 Cable Television Laboratories, Inc.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS ``AS
 * IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL CABLE TELEVISION LABS INC. OR ITS
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

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
