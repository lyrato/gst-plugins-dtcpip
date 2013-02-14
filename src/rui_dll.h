// COPYRIGHT_BEGIN
//  DO NOT ALTER OR REMOVE COPYRIGHT NOTICES OR THIS FILE HEADER
//  
//  Copyright (C) 2008-2009, Cable Television Laboratories, Inc. 
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
#if !defined(_RUI_DLL_H)
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
 * This populates the porting layer with the global function table pointer
 * so that it can be passed to library modules during intialization of the
 * modules.
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
 * @param name is a pointer to the name of the module to open.
 * @param dlmodId is a pointer for returning the identifier of the opened module.
 * @return The MPE error code if the create fails, otherwise <i>MPE_SUCCESS<i/> 
 *          is returned.
 */
gboolean rui_dlmodOpen(const char *name, rui_Dlmod *dlmodId);

/**
 * The <i>rui_dlmodClose()</i> function shall terminate use of the target
 * module.  The target module identifier is the identifier returned from the
 * original library open call.
 *
 * @param dlmodId is the identifier of the target module.
 * @return The MPE error code if the create fails, otherwise <i>MPE_SUCCESS<i/> 
 *          is returned.
 */
gboolean rui_dlmodClose(rui_Dlmod dlmodId);

/**
 * The <i>rui_dlmodGetSymbol()</i> function shall locate symbol information for
 * target symbol in specified module.  This will be the mechanism for locating a 
 * target function within a module.  The target library module is specified by the 
 * ID/handle returned from the "open" operation.
 *
 * @param dlmodId is the identifier of the target module.
 * @param symbol is a pointer to a name string for the symbol for which to perform 
 *          the search/lookup.
 * @param value is a void pointer for returning the associated value of the target
 *          symbol.
 * @return The MPE error code if the create fails, otherwise <i>MPE_SUCCESS<i/> 
 *          is returned.
 */
gboolean rui_dlmodGetSymbol(rui_Dlmod dlmodId, const char *symbol,
        void **value);

#ifdef __cplusplus
}
#endif
#endif /* __DLL_H */
