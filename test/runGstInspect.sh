#!/bin/sh
GST_PLUGIN_PATH=/usr/local/lib/gstreamer-1.0
#GST_PLUGIN_PATH=/usr/local/lib/gstreamer-0.10
export GST_PLUGIN_PATH
#
#RUIH_GST_DTCP_DISABLE=true
#export RUIH_GST_DTCP_DISABLE
#
RUIH_GST_DTCP_KEY_STORAGE=/home/landerson/RUIHRI/git/gst-plugins-cl/dtcpip/test
#RUIH_GST_DTCP_KEY_STORAGE=/media/truecrypt2/dll/prod_keys
export RUIH_GST_DTCP_KEY_STORAGE
#
RUIH_GST_DTCP_DLL=/home/landerson/RUIHRI/git/gst-plugins-cl/dtcpip/test/dtcpip_mock.so
#RUIH_GST_DTCP_DLL=/media/truecrypt2/dtcpip_v1.1_prod.so
export RUIH_GST_DTCP_DLL
#
LD_LIBRARY_PATH=/usr/local/lib;
export LD_LIBRARY_PATH
#
GST_DEBUG=*:3
export GST_DEBUG
#
gst-inspect-1.0 --gst-plugin-spew dtcpip
#gst-inspect-0.10 --gst-plugin-spew dtcpip
