EXTRA_DIST = autogen.sh

# Note: plugindir is set in configure

##############################################################################
# TODO: change libgstplugin.la to something else, e.g. libmysomething.la     #
##############################################################################
plugin_LTLIBRARIES = src/libgstdtcpip.la

##############################################################################
# TODO: for the next set of variables, name the prefix if you named the .la, #
#  e.g. libmysomething.la => libmysomething_la_SOURCES                       #
#                            libmysomething_la_CFLAGS                        #
#                            libmysomething_la_LIBADD                        #
#                            libmysomething_la_LDFLAGS                       #
##############################################################################

# sources used to compile this plug-in
src_libgstdtcpip_la_SOURCES = src/gstdtcpip.c src/gstdtcpip.h src/rui_dtcpip.h src/rui_dtcpip.c src/rui_dll.h src/rui_dll.c

# compiler and linker flags used to compile this plugin, set in configure.ac
src_libgstdtcpip_la_CFLAGS = $(GST_CFLAGS)
src_libgstdtcpip_la_LIBADD = $(GST_LIBS)
src_libgstdtcpip_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS)
src_libgstdtcpip_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = src/gstdtcpip.h src/rui_dtcpip.h src/rui_dll.h
