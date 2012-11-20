#ifndef _RUI_DTCPIP_H_
#define _RUI_DTCPIP_H_

#include <stdlib.h> // size_t

#define DTCPIP_INVALID_SESSION_HANDLE (-1)
#define DTCPIP_IS_SESSION_VALID(h) ((h)>0)

#define DTCPIP_SUCCESS (0)
// dtcpip (CableLabs) error code range -1 through -4
// DTCP (Intel) error code range is below -100
#define IS_DTCPIP_SUCCESS(ret_val) ((ret_val)>=0)
#define IS_DTCPIP_FAILURE(ret_val) ((ret_val)<0)

extern int dtcpip_cmn_init(const char* storage_path);
extern void dtcpip_cmn_get_version(char* string, size_t length);

extern int dtcpip_snk_init(void);
extern int dtcpip_snk_open(char* ip_addr, unsigned short ip_port, int *session_handle);
extern int dtcpip_snk_alloc_decrypt(int session_handle,
               char* encrypted_data, size_t encrypted_size,
               char** cleartext_data, size_t* cleartext_size);
               int dtcpip_snk_free(char* cleartext_data);
extern int dtcpip_snk_close(int session_handle);

extern int dtcpip_src_init(unsigned short dtcp_port);
extern int dtcpip_src_open(int* session_handle, int is_audio_only);
extern int dtcpip_src_alloc_encrypt(int session_handle,
               unsigned char cci,
               char* cleartext_data, size_t cleartext_size,
               char** encrypted_data, size_t* encrypted_size);
extern int dtcpip_src_free(char* encrypted_data);
extern int dtcpip_src_close(int session_handle);

#endif // _RUI_DTCPIP_H_
