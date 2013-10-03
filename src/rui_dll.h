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

#ifndef _RUI_DLL_H_
#define _RUI_DLL_H

#include <gst/gst.h>

#ifdef __cplusplus
extern "C"
{
#endif

/***
 * Dynamically linked library type definitions:
 */
typedef void* rui_Dlmod; /* DLL identifier type. */
typedef void* rui_DlmodData; /* DLL data structure type (used for allocation). */

/***
 * Dynamically linked library API prototypes:
 */

/**
 * <i>rui_dlmodInit</i>
 *
 * Initialize the rui DLL support with the global function table (rui_ftable).
 * This populates the global function table pointer so that it can be passed to
 * library modules during initialization of the modules.
 *
 * @param rui_ftable is a pointer to the global function table.
 */
void rui_dlmodInit(void **rui_ftable);

/**
 * The <i>rui_dlmodOpen()</i> function shall load/locate/link and initialize the
 * module specified by name.  The module may be loaded from a file system or 
 * located in RAM or ROM.  This function can also be used to get a pointer to the 
 * calling module (null name).  This initialization interface returns a ID/handle 
 * for association of the library module and subsequent symbol lookup.
 *
 * @param name is the name of the module to open.
 * @param dlmodId is a pointer for returning the identifier of the opened module.
 * @return FALSE if the create fails, otherwise TRUE is returned.
 */
gboolean rui_dlmodOpen(const char *name, rui_Dlmod *dlmodId);

/**
 * The <i>rui_dlmodClose()</i> function shall terminate use of the target
 * module.  The target module identifier is the identifier returned from the
 * original library open call.
 *
 * @param dlmodId is the identifier of the target module.
 * @return false if DLL close fails, otherwise true is returned.
 */
gboolean rui_dlmodClose(rui_Dlmod dlmodId);

/**
 * The <i>rui_dlmodGetSymbol()</i> function shall locate symbol information for
 * target symbol in specified module.  This will be the mechanism for locating a 
 * target function within a module.  The target library module is specified by the 
 * ID/handle returned from the "open" operation.
 *
 * @param dlmodId is the identifier of the target module.
 * @param symbol is string containing name of the symbol for which to perform
 *          the search/lookup.
 * @param value is a void pointer for returning the associated value of the target
 *          symbol.
 * @return FALSE if fails to get symbol, otherwise TRUE is returned.
 */
gboolean rui_dlmodGetSymbol(rui_Dlmod dlmodId, const char *symbol,
        void **value);

#ifdef __cplusplus
}
#endif
#endif /* __DLL_H */
