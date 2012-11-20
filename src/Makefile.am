# Note: plugindir is set in configure

##############################################################################
# TODO: change libgstdtcpip.la to something else, e.g. libmysomething.la     #
##############################################################################
plugin_LTLIBRARIES = libgstdtcpip.la

##############################################################################
# TODO: for the next set of variables, name the prefix if you named the .la, #
#  e.g. libmysomething.la => libmysomething_la_SOURCES                       #
#                            libmysomething_la_CFLAGS                        #
#                            libmysomething_la_LIBADD                        #
#                            libmysomething_la_LDFLAGS                       #
##############################################################################

# sources used to compile this plug-in
libgstdtcpip_la_SOURCES = gstdtcpip.c gstdtcpip.h rui_dtcpip.h

# compiler and linker flags used to compile this plugin, set in configure.ac
libgstdtcpip_la_CFLAGS = $(GST_CFLAGS)
libgstdtcpip_la_CXXFLAGS = $(GST_CFLAGS)
libgstdtcpip_la_LIBADD = $(GST_LIBS)
libgstdtcpip_la_LDFLAGS = $(GST_PLUGIN_LDFLAGS) -L/media/truecrypt1/dtcp-ip_sik -l:dtcpip.so
libgstdtcpip_la_LIBTOOLFLAGS = --tag=disable-static

# headers we need but don't want installed
noinst_HEADERS = gstdtcpip.h rui_dtcpip.h
