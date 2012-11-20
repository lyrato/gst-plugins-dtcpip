/*
 * GStreamer
 * Copyright (C) 2005 Thomas Vander Stichele <thomas@apestaart.org>
 * Copyright (C) 2005 Ronald S. Bultje <rbultje@ronald.bitfreak.net>
 * Copyright (C) 2012 Doug Young <D.Young@cablelabs.com>
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 *
 * Alternatively, the contents of this file may be used under the
 * GNU Lesser General Public License Version 2.1 (the "LGPL"), in
 * which case the following provisions apply instead of the ones
 * mentioned above:
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:element-dtcpip
 *
 * DTCP (Digital Transmission Content Protection) plugins.
 *
 * <refsect2>
 * <title>Example launch line</title>
 * |[
 * gst-launch -v -m fakesrc ! dtcpipdec dtcp1host=10.4.19.241 dtcp1port=8999 ! fakesink silent=TRUE
 * ]|
 * </refsect2>
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gst/gst.h>
#include <string.h>

#include "gstdtcpip.h"
#include "rui_dtcpip.h"

#define GST_CAT_DEFAULT gst_dtcpipdec_debug
GST_DEBUG_CATEGORY_STATIC (GST_CAT_DEFAULT);

enum
{
  PROP_0,
  PROP_SILENT,
  PROP_DTCP1HOST,
  PROP_DTCP1PORT,
  PROP_DTCPIP_STORAGE
};

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE ("sink",
    GST_PAD_SINK,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("application/x-dtcp1")
    );

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE ("src",
    GST_PAD_SRC,
    GST_PAD_ALWAYS,
    GST_STATIC_CAPS ("ANY")
    );

GST_BOILERPLATE (GstDtcpIpDec, gst_dtcpipdec, GstElement,
    GST_TYPE_ELEMENT);

static void gst_dtcpipdec_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec);
static void gst_dtcpipdec_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec);

static gboolean gst_dtcpipdec_set_caps (GstPad * pad, GstCaps * caps);
static GstFlowReturn gst_dtcpipdec_chain (GstPad * pad, GstBuffer * buf);
static GstStateChangeReturn gst_dtcpipdec_change_state (GstElement *element,
    GstStateChange transition);

/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 * the name (plugin_init) is specified in GST_PLUGIN_DEFINE at bottom
 */
static gboolean
plugin_init (GstPlugin * plugin)
{
//printf(">>> dtcpipdec: plugin_init: %s: \"%s\"\n",
//    gst_plugin_get_name(plugin),
//    gst_plugin_get_description(plugin));
  /* debug category for fltering log messages */
  GST_DEBUG_CATEGORY_INIT (GST_CAT_DEFAULT, "dtcpip",
      0, "DTCP-IP library diagnostic output");

  return gst_element_register (plugin, "dtcpipdec", GST_RANK_NONE,
      GST_TYPE_DTCPIP);
}

/* GObject vmethod implementations */
static void
gst_dtcpipdec_base_init (gpointer gclass)
{
//printf(">>> gst_dtcpipdec_base_init\n");
  GstElementClass *element_class = GST_ELEMENT_CLASS (gclass);

  gst_element_class_set_details_simple(element_class,
    "DTCP-IP decryption",
    "Decrypt/DTCP", // see docs/design/draft-klass.txt
    "Decrypts link-encrypted DTCP-IP DLNA content",
    "Doug Young <D.Young@cablelabs.com> 11/19/12 8:09 PM");

  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&src_factory));
  gst_element_class_add_pad_template (element_class,
      gst_static_pad_template_get (&sink_factory));
}

/* initialize the dtcpip's class */
static void
gst_dtcpipdec_class_init (GstDtcpIpDecClass * klass)
{
//printf(">>> gst_dtcpipdec_class_init\n");
  GObjectClass *gobject_class = (GObjectClass *) klass;
  GstElementClass *gstelement_class = (GstElementClass *) klass;

  //gobject_class = (GObjectClass *) klass;
  //gstelement_class = (GstElementClass *) klass;

  gobject_class->set_property = gst_dtcpipdec_set_property;
  gobject_class->get_property = gst_dtcpipdec_get_property;

  g_object_class_install_property (gobject_class, PROP_SILENT,
      g_param_spec_boolean ("silent", "Silent", "Produce verbose output ?",
      FALSE, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_DTCP1HOST,
      g_param_spec_string ("dtcp1host", "dtcp1host",
      "Host name or IP address for DTCP AKE negotiation",
      "128.0.0.1", G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_DTCP1PORT,
      g_param_spec_uint ("dtcp1port", "dtcp1port",
      "Host port number for DTCP AKE negotiation",
      0, 65535, 8999, G_PARAM_READWRITE));

  g_object_class_install_property (gobject_class, PROP_DTCPIP_STORAGE,
      g_param_spec_string ("dtcpip_storage", "dtcpip_storage",
      "Directory that contains client's keys",
      "/tmp", G_PARAM_READWRITE));

  gstelement_class->change_state = gst_dtcpipdec_change_state;
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void
gst_dtcpipdec_init (GstDtcpIpDec * filter,
    GstDtcpIpDecClass * gclass)
{
//printf(">>> gst_dtcpipdec_init\n");

  filter->sinkpad = gst_pad_new_from_static_template (&sink_factory, "sink");
  gst_pad_set_setcaps_function (filter->sinkpad,
                                GST_DEBUG_FUNCPTR(gst_dtcpipdec_set_caps));
  gst_pad_set_getcaps_function (filter->sinkpad,
                                GST_DEBUG_FUNCPTR(gst_pad_proxy_getcaps));
  gst_pad_set_chain_function (filter->sinkpad,
                              GST_DEBUG_FUNCPTR(gst_dtcpipdec_chain));

  filter->srcpad = gst_pad_new_from_static_template (&src_factory, "src");
  gst_pad_set_getcaps_function (filter->srcpad,
                                GST_DEBUG_FUNCPTR(gst_pad_proxy_getcaps));

  gst_element_add_pad (GST_ELEMENT (filter), filter->sinkpad);
  gst_element_add_pad (GST_ELEMENT (filter), filter->srcpad);

  // Initialize element properties
  filter->silent = FALSE;
  filter->dtcp1host = NULL;
  filter->dtcp1port = -1;
  filter->dtcpip_storage = NULL;

  // Initialize DTCP instance variable
  filter->session_handle = -1;
}

static void
gst_dtcpipdec_set_property (GObject * object, guint prop_id,
    const GValue * value, GParamSpec * pspec)
{
//printf(">>> gst_dtcpipdec_set_property\n");
  GstDtcpIpDec *filter = GST_DTCPIP (object);

  switch (prop_id) {
    case PROP_SILENT:
      filter->silent = g_value_get_boolean (value);
      break;
    case PROP_DTCP1HOST:
      // I hope this is the way to free a dup'd string -dougy
      if (filter->dtcp1host) g_free(filter->dtcp1host);
      filter->dtcp1host = g_value_dup_string (value);
      break;
    case PROP_DTCP1PORT:
      filter->dtcp1port = g_value_get_uint (value);
      break;
    case PROP_DTCPIP_STORAGE:
      // I hope this is the way to free a dup'd string -dougy
      if (filter->dtcpip_storage) g_free(filter->dtcpip_storage);
      filter->dtcpip_storage = g_value_dup_string (value);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

static void
gst_dtcpipdec_get_property (GObject * object, guint prop_id,
    GValue * value, GParamSpec * pspec)
{
//printf(">>> gst_dtcpipdec_get_property\n");
  GstDtcpIpDec *filter = GST_DTCPIP (object);

  switch (prop_id) {
    case PROP_SILENT:
      g_value_set_boolean (value, filter->silent);
      break;
    case PROP_DTCP1HOST:
      g_value_set_string (value, filter->dtcp1host);
      break;
    case PROP_DTCP1PORT:
      g_value_set_uint (value, filter->dtcp1port);
      break;
    case PROP_DTCPIP_STORAGE:
      g_value_set_string (value, filter->dtcpip_storage);
      break;
    default:
      G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
      break;
  }
}

/* GstElement vmethod implementations */

/* this function handles the link with other elements */
static gboolean
gst_dtcpipdec_set_caps (GstPad * pad, GstCaps * caps)
{
  GstDtcpIpDec *filter = GST_DTCPIP (gst_pad_get_parent (pad));
  GstPad *otherpad;

  otherpad = (pad == filter->srcpad) ? filter->sinkpad : filter->srcpad;
  gst_object_unref (filter);

  return gst_pad_set_caps (otherpad, caps);
}

/* Element state change
 *
 */
static GstStateChangeReturn
gst_dtcpipdec_change_state (GstElement *element, GstStateChange transition)
{
  int ret_val = DTCPIP_SUCCESS;
  GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
  GstDtcpIpDec *filter = GST_DTCPIP (element);
  char dtcpip_version[1024];

  if (filter->silent == FALSE)
    g_message ("dtcpipdec: gst_dtcpipdec_change_state: GST_STATE_CHANGE_%s_TO_%s",
      gst_element_state_get_name(GST_STATE_TRANSITION_CURRENT(transition)),
      gst_element_state_get_name(GST_STATE_TRANSITION_NEXT(transition)) ); 
       
  switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
      /* Allocate non-stream-specific resources (libs, mem) */
      ret_val = dtcpip_cmn_init(filter->dtcpip_storage);
      if ( filter->silent==FALSE ) {
        g_message("dtcpip_cmn_init(\"%s\")", filter->dtcpip_storage);
        dtcpip_cmn_get_version(dtcpip_version, sizeof(dtcpip_version));
        g_message("dtcpip_cmn_get_version\"=%s\"", dtcpip_version);
        g_message("dtcpip_snk_init()");
      }
      ret_val = dtcpip_snk_init();
      break;
    case GST_STATE_CHANGE_READY_TO_PAUSED:
      /* Allocate stream-specific resources */
      ret_val = dtcpip_snk_open(filter->dtcp1host, filter->dtcp1port, &(filter->session_handle));
      if (filter->silent == FALSE)
        g_message ("dtcpipdec: gst_dtcpipdec_change_state: dtcpip_snk_open(%s,%d), ret_val=%d",
          filter->dtcp1host, filter->dtcp1port, ret_val);
       break;
    //case GST_STATE_CHANGE_PAUSED_TO_PLAYING // same as PAUSED
	  //  /* Most elements ignore this state change. */
    //  break;
    default:
      break;
  }
  
  ret = GST_ELEMENT_CLASS (parent_class)->change_state (element, transition);
  if (ret == GST_STATE_CHANGE_FAILURE) return ret;
    
  switch (transition) {
   // case GST_STATE_CHANGE_PLAYING_TO_PAUSED:
   //   /* Most elements ignore this state change. */
   //   break;
    case GST_STATE_CHANGE_PAUSED_TO_READY:
      /* De-llocate stream-specific resources */
      ret_val = dtcpip_snk_close(filter->session_handle);
      if (filter->silent == FALSE)
        g_message ("dtcpipdec: gst_dtcpipdec_change_state: dtcpip_snk_close(), ret_val=%d", ret_val);
      filter->session_handle = DTCPIP_INVALID_SESSION_HANDLE;
      break;
    case GST_STATE_CHANGE_READY_TO_NULL:
      /* De-allocate non-stream-specific resources (libs, mem) */
      break;
    default:
      break;
    }
    
  return ret;
}

/* chain function
 * this function does the actual processing
 */
static GstFlowReturn // GST_FLOW_OK, GST_FLOW_ERROR
gst_dtcpipdec_chain (GstPad * pad, GstBuffer * inbuf)
{
  int ret_val;
  GstDtcpIpDec *filter;
  char *encrypted_data, *cleartext_data;
  size_t encrypted_size, cleartext_size;
  GstBuffer *outbuf;

  filter = GST_DTCPIP (GST_OBJECT_PARENT (pad));
  if (!filter->silent)
    g_message ("dtcpipdec: gst_dtcpipdec_chain: buffer %p, %u bytes",
      GST_BUFFER_DATA(inbuf), GST_BUFFER_SIZE(inbuf));

  // 1. set our encrypted data pointer
  encrypted_data = (char*)GST_BUFFER_DATA(inbuf);
  encrypted_size = GST_BUFFER_SIZE(inbuf);
  // 2. Call the DTCPIP decryption
  ret_val = dtcpip_snk_alloc_decrypt(filter->session_handle,
    encrypted_data, encrypted_size,
    &cleartext_data, &cleartext_size);
  if ( !filter->silent && IS_DTCPIP_FAILURE(ret_val) )
    g_message ("dtcpipdec: gst_dtcpipdec_chain: dtcpip_snk_alloc_decrypt(), ret_val=%d", ret_val);
  gst_buffer_unref(inbuf);
  // 3. Create a newly allocated buffer (refcount=1) without any data
  outbuf = gst_buffer_new();
  // 4. Set the new buffer's data to be the decrypted cleartest, using gst_buffer_set_data
  gst_buffer_set_data(outbuf, (guint8*)cleartext_data, cleartext_size);
  // 5. push the data to our sink pad, and onto the downstream element
  GstFlowReturn gfr = gst_pad_push (filter->srcpad, outbuf);
  // 6. Free the cleartext buffer that was allocated implicitly
  ret_val = dtcpip_snk_free(cleartext_data);
  if ( !filter->silent && IS_DTCPIP_FAILURE(ret_val) )
    g_message ("dtcpipdec: gst_dtcpipdec_chain: dtcpip_snk_free(), ret_val=%d", ret_val);

  // XXX. Calling unref on our outbuf causes the following down-stream assertion:
  // GStreamer-CRITICAL **: gst_mini_object_unref: assertion `GST_IS_MINI_OBJECT (mini_object)' failed
  //gst_buffer_unref(outbuf);

  return gfr; //gst_pad_push (filter->srcpad, buf);
}

/* PACKAGE: this is usually set by autotools depending on some _INIT macro
 * in configure.ac and then written into and defined in config.h, but we can
 * just set it ourselves here in case someone doesn't use autotools to
 * compile this code. GST_PLUGIN_DEFINE needs PACKAGE to be defined.
 */
#ifndef PACKAGE
#define PACKAGE "cl-plugins"
#endif

/* gstreamer looks for this structure to register the dtcpip plugin
 *
 * exchange the string 'Template dtcpip' with your dtcpip description
 */
GST_PLUGIN_DEFINE (
    GST_VERSION_MAJOR,
    GST_VERSION_MINOR,
    "dtcpip",
    "DTCP (Digital Transmission Content Protection) plugins",
    plugin_init,
    VERSION,
    "LGPL", // should be "proprietary", but that gets us blacklisted
    "CableLabs RUI-H RI",
    "http://www.cablelabs.com/"
)
