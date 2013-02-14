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
#include <dlfcn.h>    /* dlopen(3), dlerror(3), dlsym(3), dlclose(3) */

#include <rui_dll.h>

static void **g_rui_ftable; /* RUI Global Function Table */

/**
 * <i>rui_dlmodInit</i>
 *
 * Initialize the rui DLL support with the rui global function table
 * (rui_ftable).  This populates the porting layer with the rui global
 * function table pointer so it can be passed to library modules during
 * intialization of the modules.
 *
 * @param rui_ftable pointer to rui global function table.
 */
void rui_dlmodInit(void **rui_ftable)
{
    printf("%s - assigning function table\n", __FUNCTION__);
    g_rui_ftable = rui_ftable;
}

/**
 * The <i>rui_dlmodOpen()</i> function shall load/locate/link and
 * initialize the module specified by name.  The module may be loaded
 * from a file system or located in RAM or ROM.  This function can also
 * be used to get a pointer to the calling module (null name).  This
 * initialization interface returns a ID/handle for association of the
 * library module and subsequent symbol lookup.
 *
 * TODO - identify the Linux module-loading mechanism
 *
 * In the Linux implementation, the module is loaded using TBD.
 * Then the shared library symbol table is acquired via a MDT entry in
 * the module.  This symbol table is a hash table that is subsequently
 * used for symbol lookups.
 *
 * @param name is a pointer to the name of the module to open.
 * @param dlmodId is a pointer for returning the identifier of the
 *     opened module.
 * @return false if the create fails, otherwise true is returned.
 */
gboolean rui_dlmodOpen(const char *name, rui_Dlmod *dlmodId)
{
    printf("%s - initializing\n", __FUNCTION__);

    //void (*mpelib_init)(void**); /* Library init routine. */
    void *linuxMod;

    if (NULL == name)
    {
        g_printerr("%s - module name is NULL!\n", __FUNCTION__);
        return FALSE;
    }

    printf("%s - opening module name: \"%s\"\n", __FUNCTION__, name);

    /*
     * open the dynamic library in Linux.
     * for now, let's open the DLL in LAZY mode, meaning
     * resolve symbols as the DLL is executed.
     */
    if ((linuxMod = dlopen(name, RTLD_LAZY)) == NULL)
    {
        // This log mesage is DEBUG level because some clients use this failure
        // as a test for library existence
        g_printerr("%s() - dlopen('%s') failed: %s\n", __FUNCTION__, name, dlerror());
        return FALSE;
    }
    *dlmodId = linuxMod;

    printf("%s - SUCCESS! Handle = %p\n", __FUNCTION__, *dlmodId);

    /*
     * The library's rui initialization entry point is optional.
     * If it exists, it must be of the form ruilib_init_[name],
     * where "name" corresponds to the name of the library.
     */
    /*
    if (TRUE == rui_dlmodGetSymbol(*dlmodId, "mpelib_init", (void**) &mpelib_init))
    {
        printf("%s - invoking library rui initialization function: 'mpelib_init' @ 0x%p\n",
                __FUNCTION__, mpelib_init);
        (*mpelib_init)(g_rui_ftable);
    }
    else
    {
        printf("%s - NOT invoking library rui initialization function: 'ruilib_init' (symbol not found)\n",
        		__FUNCTION__);
    }
    */
    return TRUE;
}

/**
 * The <i>rui_dlmodGetSymbol()</i> function shall locate symbol
 * information for target symbol in specified module.  This will be the
 * mechanism for locating a target function within a module.  The target
 * library module is specified by the ID/handle returned from the "open"
 * operation.
 *
 * @param dlmodId is the identifier of the target module.
 * @param symbol is a pointer to a name string for the symbol for which
 *     to perform the search/lookup.
 * @param value is a void pointer for returning the associated value of
 * the target symbol.
 * @return FALSE if the create fails, otherwise TRUE is returned.
 */
gboolean rui_dlmodGetSymbol(rui_Dlmod dlmodId, const char *symbol,
        					void **value)
{
    char *checkRet = (char *) 0;

    /*
     * Per Linux Manpage
     * 1. Clear any extant errors
     * 2. Search for the symbol (NULL is legitimate return value)
     * 3. Check for resulting error
     */
    (void) dlerror();
    *value = dlsym(dlmodId, symbol);
    checkRet = dlerror();
    if (NULL != checkRet)
    {
        /*
         * Changed from _ERROR to _DEBUG to eliminate "noise".
         * The caller should log a complaint, if appropriate.
         */
        g_printerr("%s - failed to find symbol \"%s\" [dll_handle %p].  Reason: %s\n",
                __FUNCTION__, symbol, dlmodId, checkRet);
        return FALSE;
    }

    printf("%s - found symbol \"%s\"\n", __FUNCTION__, symbol);

    return TRUE;
}

/**
 * <i>rui_dlmodClose()</i>
 *
 * Unlink and deinitialize a dynamic link library.
 *
 * @param dlmodId is the DLL handle to close.
 *
 * @return false if DLL close fails, otherwise true is returned.
 */
gboolean rui_dlmodClose(rui_Dlmod dlmodId)
{
    printf("%s - %p\n", __FUNCTION__, dlmodId);

    /* close the DLL in Linux. */
    dlclose(dlmodId);

    return TRUE;
}
