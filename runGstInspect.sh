#!/bin/sh
GST_PLUGIN_PATH=/usr/local/lib/gstreamer-0.10
gst-inspect --gst-debug-level=1 --gst-debug=dtcpipdec:5 --gst-plugin-load=$GST_PLUGIN_PATH/libgstdtcpip.so dtcpipdec
