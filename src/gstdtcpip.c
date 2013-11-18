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
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <string.h>
#include <gst/gst.h>
#include "gstdtcpip.h"
#include "rui_dtcpip.h"
#include <glib/gprintf.h>

// Uncomment to have output buffers saved to file
//#define DEBUG_SAVE_BUFFER_CONTENT

GST_DEBUG_CATEGORY(gst_dtcpip_debug);
#define GST_CAT_DEFAULT gst_dtcpip_debug

enum {
    PROP_0, PROP_DTCP1HOST, PROP_DTCP1PORT, PROP_DTCPIP_STORAGE
};

#define RUIH_GST_DTCP_DISABLE "RUIH_GST_DTCP_DISABLE"

#define RTLD_NOW    0x00002     /* Immediate function call binding.  */

#ifdef DEBUG_SAVE_BUFFER_CONTENT
static FILE* g_debugBufferFile = NULL;
static gchar* g_debugBufferFileName = "buffers.txt";
#endif

/* the capabilities of the inputs and outputs.
 *
 * describe the real formats here.
 */
static GstStaticPadTemplate sink_factory = GST_STATIC_PAD_TEMPLATE("sink",
        GST_PAD_SINK, GST_PAD_ALWAYS,
        GST_STATIC_CAPS("application/x-dtcp1"));

static GstStaticPadTemplate src_factory = GST_STATIC_PAD_TEMPLATE("src",
        GST_PAD_SRC, GST_PAD_ALWAYS, GST_STATIC_CAPS("ANY"));

#define gst_dtcpip_parent_class parent_class
G_DEFINE_TYPE(GstDtcpIp, gst_dtcpip, GST_TYPE_BASE_TRANSFORM);

static void gst_dtcpip_set_property(GObject * object, guint prop_id,
        const GValue * value, GParamSpec * pspec);
static void gst_dtcpip_get_property(GObject * object, guint prop_id,
        GValue * value, GParamSpec * pspec);

//static gboolean gst_dtcpip_pad_event(GstPad *pad, GstObject *parent,
//        GstEvent *event);

//static GstFlowReturn gst_dtcpip_chain(GstPad * pad, GstObject* parent,
//        GstBuffer * buf);
static GstFlowReturn gst_dtcpip_transform (GstBaseTransform *trans,
    GstBuffer * in_buf, GstBuffer * out_buf);

static GstStateChangeReturn gst_dtcpip_change_state(GstElement *element,
        GstStateChange transition);

static GstCaps *gst_dtcpip_transform_caps (GstBaseTransform *
    trans, GstPadDirection direction, GstCaps * caps, GstCaps * filter);

static void
gst_dtcpip_finalize (GObject * obj)
{
  //GstDtcpIp *filter = GST_DTCPIP (obj);

  /* *todo* - free things up */
  G_OBJECT_CLASS (parent_class)->finalize (obj);
}

/* initialize the dtcpip's class */
static void gst_dtcpip_class_init(GstDtcpIpClass * klass) {
    GObjectClass *gobject_class = (GObjectClass *) klass;
    GstElementClass *element_class = (GstElementClass *) klass;
    GstBaseTransformClass *trans_class = GST_BASE_TRANSFORM_CLASS (klass);

    gobject_class->set_property = gst_dtcpip_set_property;
    gobject_class->get_property = gst_dtcpip_get_property;
    gobject_class->finalize = gst_dtcpip_finalize;

    g_object_class_install_property(gobject_class, PROP_DTCP1HOST,
            g_param_spec_string("dtcp1host", "dtcp1host",
                    "Host name or IP address for DTCP AKE negotiation",
                    "128.0.0.1", G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, PROP_DTCP1PORT,
            g_param_spec_uint("dtcp1port", "dtcp1port",
                    "Host port number for DTCP AKE negotiation", 0, 65535, 8999,
                    G_PARAM_READWRITE));

    g_object_class_install_property(gobject_class, PROP_DTCPIP_STORAGE,
            g_param_spec_string("dtcpip_storage", "dtcpip_storage",
                    "Directory that contains client's keys",
                    "/media/truecrypt1/dll/test_keys", G_PARAM_READABLE));
/*
    gst_element_class_set_details_simple(gstelement_class, "DTCP-IP decryption",
            "Decrypt/DTCP", // see docs/design/draft-klass.txt
            "Decrypts link-encrypted DTCP-IP DLNA content",
            "Doug Young <D.Young@cablelabs.com> 2/19/12 11:45 AM");
*/
    gst_element_class_set_static_metadata (element_class,
        "DTCP/IP Decrypter", "Decoder",
        "Decrypts DTCP/IP encrypted audio/video content",
        "GStreamer maintainers <gstreamer-devel@lists.sourceforge.net>");

    gst_element_class_add_pad_template(element_class,
            gst_static_pad_template_get(&src_factory));
    gst_element_class_add_pad_template(element_class,
            gst_static_pad_template_get(&sink_factory));

    /* *TODO* - do we need state change method */
    element_class->change_state = gst_dtcpip_change_state;

    trans_class->passthrough_on_same_caps = FALSE;
    trans_class->transform = gst_dtcpip_transform;
    trans_class->transform_caps = gst_dtcpip_transform_caps;
}

/* initialize the new element
 * instantiate pads and add them to element
 * set pad calback functions
 * initialize instance structure
 */
static void gst_dtcpip_init(GstDtcpIp * filter) {
    GST_DEBUG_OBJECT (filter, "Initializing");

    // Initialize sink pad
    //filter->sinkpad = gst_pad_new_from_static_template(&sink_factory, "sink");

    //gst_pad_set_event_function(filter->sinkpad,
    //        GST_DEBUG_FUNCPTR(gst_dtcpip_pad_event));

    //gst_pad_set_chain_function(filter->sinkpad,
    //        GST_DEBUG_FUNCPTR(gst_dtcpip_chain));

    //GST_PAD_SET_PROXY_CAPS(filter->sinkpad);
    //gst_element_add_pad(GST_ELEMENT(filter), filter->sinkpad);

    // Initialize src pad
    //filter->srcpad = gst_pad_new_from_static_template(&src_factory, "src");
    //GST_PAD_SET_PROXY_CAPS(filter->srcpad);
    //gst_element_add_pad(GST_ELEMENT(filter), filter->srcpad);

    // Initialize element properties
    filter->dtcp1host = NULL;
    filter->dtcp1port = -1;

    // Read env vars to get disable flag, storage path and key file name
    filter->dtcp_disabled = FALSE;

    if (getenv(RUIH_GST_DTCP_DISABLE)) {
        GST_WARNING_OBJECT(filter, "Disabling DTCP due to env var set: %s",
                RUIH_GST_DTCP_DISABLE);
        filter->dtcp_disabled = TRUE;
    } else {
        GST_INFO_OBJECT(filter,
                "DTCP is enabled due to disable env var NOT set: %s",
                RUIH_GST_DTCP_DISABLE);

        GST_INFO_OBJECT(filter, "Initializing shared library");
        if (!rui_dtcpip_init()) {
            GST_ERROR_OBJECT(filter, "Problems initializing shared library");
        } else {
            GST_INFO_OBJECT(filter, "Successfully initialized library");
        }
    }

    // Initialize DTCP instance variable
    filter->session_handle = -1;

#ifdef DEBUG_SAVE_BUFFER_CONTENT
    g_debugBufferFile = g_fopen(g_debugBufferFileName, "wb");
#endif

}

static void gst_dtcpip_set_property(GObject * object, guint prop_id,
        const GValue * value, GParamSpec * pspec) {
    GstDtcpIp *filter = GST_DTCPIP (object);

    GST_LOG_OBJECT(filter, "Setting property: %d", prop_id);
    switch (prop_id) {
    case PROP_DTCP1HOST:
        if (filter->dtcp1host) {
            g_free(filter->dtcp1host);
        }
        filter->dtcp1host = g_value_dup_string(value);
        GST_INFO_OBJECT(filter, "Host property: %s", filter->dtcp1host);
        break;
    case PROP_DTCP1PORT:
        filter->dtcp1port = g_value_get_uint(value);
        GST_INFO_OBJECT(filter, "Port property: %d", filter->dtcp1port);
        break;
    default:
        GST_INFO_OBJECT(filter, "Unknown property id: %d", prop_id);
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

static void gst_dtcpip_get_property(GObject * object, guint prop_id,
        GValue * value, GParamSpec * pspec) {
    GstDtcpIp *filter = GST_DTCPIP (object);
    GST_LOG_OBJECT(filter, "Getting property");

    switch (prop_id) {
    case PROP_DTCP1HOST:
        g_value_set_string(value, filter->dtcp1host);
        break;
    case PROP_DTCP1PORT:
        g_value_set_uint(value, filter->dtcp1port);
        break;
    case PROP_DTCPIP_STORAGE:
        g_value_set_string(value, filter->dtcpip_storage);
        break;
    default:
        G_OBJECT_WARN_INVALID_PROPERTY_ID(object, prop_id, pspec);
        break;
    }
}

/*
static gboolean gst_dtcpip_pad_event(GstPad *pad, GstObject *parent,
        GstEvent *event) {
    gboolean ret;
    GstDtcpIp *filter = GST_DTCPIP(parent);

    switch (GST_EVENT_TYPE(event)) {
    case GST_EVENT_CAPS: {
        GstCaps * caps;

        gst_event_parse_caps(event, &caps);
        GstPad *otherpad;

        otherpad = (pad == filter->srcpad) ? filter->sinkpad : filter->srcpad;

        gst_pad_set_caps(otherpad, caps);

         ret = gst_pad_event_default(pad, parent, event);
        break;
    }
    default:
        ret = gst_pad_event_default(pad, parent, event);
        break;
    }
    return ret;
}
*/
/* Element state change
 *
 */
static GstStateChangeReturn gst_dtcpip_change_state(GstElement *element,
        GstStateChange transition) {
    gint ret_val = DTCPIP_SUCCESS;
    GstStateChangeReturn ret = GST_STATE_CHANGE_SUCCESS;
    GstDtcpIp *filter = GST_DTCPIP (element);
    gchar dtcpip_version[1024];

    GST_INFO_OBJECT(filter, "GST_STATE_CHANGE_%s_TO_%s",
            gst_element_state_get_name(
                    GST_STATE_TRANSITION_CURRENT(transition)),
            gst_element_state_get_name(GST_STATE_TRANSITION_NEXT(transition)));

    switch (transition) {
    case GST_STATE_CHANGE_NULL_TO_READY:
        /* Allocate non-stream-specific resources (libs, mem) */
        if (!filter->dtcp_disabled) {
            g_dtcpip_ftable->dtcpip_cmn_get_version(dtcpip_version,
                    sizeof(dtcpip_version));
            GST_DEBUG_OBJECT(filter, "Got dtcpip_cmn_get_version\"=%s\"",
                    dtcpip_version);

            GST_DEBUG_OBJECT(filter, "Calling dtcpip_snk_init()");
            ret_val = g_dtcpip_ftable->dtcpip_snk_init();
            if (IS_DTCPIP_FAILURE(ret_val)) {
                GST_ERROR_OBJECT(filter,
                        "Problems in dtcpip_snk_init(\"%s\"), rc: %d",
                        filter->dtcpip_storage, ret_val);
                return GST_STATE_CHANGE_FAILURE;
            } else {
                GST_DEBUG_OBJECT(filter, "Initialized dtcpip_snk_init()");
            }
        } else {
            GST_INFO_OBJECT(filter, "Decrypting disabled");
        }
        break;

    case GST_STATE_CHANGE_READY_TO_PAUSED:
        if (!filter->dtcp_disabled) {
            /* Allocate stream-specific resources */
            ret_val = g_dtcpip_ftable->dtcpip_snk_open(filter->dtcp1host,
                    filter->dtcp1port, &(filter->session_handle));
            if (IS_DTCPIP_FAILURE(ret_val)) {
                GST_ERROR_OBJECT(filter,
                        "Problems in dtcpip_snk_open() using host: %s, port: %d, rc: %d",
                        filter->dtcp1host, filter->dtcp1port, ret_val);
                return GST_STATE_CHANGE_FAILURE;
            } else {
                GST_DEBUG_OBJECT(filter,
                        "Opened via dtcpip_snk_open(%s,%d), ret_val=%d",
                        filter->dtcp1host, filter->dtcp1port, ret_val);
            }
        }
        break;
    default:
        break;
    }

    ret = GST_ELEMENT_CLASS(parent_class)->change_state(element, transition);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        GST_ERROR_OBJECT(filter, "Problems with parent class state change");
        return ret;
    }

    switch (transition) {
    case GST_STATE_CHANGE_PAUSED_TO_READY:
        /* De-llocate stream-specific resources */
        if (!filter->dtcp_disabled) {
            ret_val = g_dtcpip_ftable->dtcpip_snk_close(filter->session_handle);
            if (IS_DTCPIP_FAILURE(ret_val)) {
                GST_ERROR_OBJECT(filter, "Problems closing");
                // Don't return error since closing anyways
            } else {
                GST_DEBUG_OBJECT(filter,
                        "Called dtcpip_snk_close(), ret_val=%d", ret_val);
            }
            filter->session_handle = DTCPIP_INVALID_SESSION_HANDLE;
        }
        break;
    case GST_STATE_CHANGE_READY_TO_NULL:
        /* De-allocate non-stream-specific resources (libs, mem) */
#ifdef DEBUG_SAVE_BUFFER_CONTENT
        fclose(g_debugBufferFile);
        g_debugBufferFile = NULL;
#endif
        break;
    default:
        break;
    }

    return ret;
}

/* chain function
 * this function does the actual processing
 */
/*
static GstFlowReturn // GST_FLOW_OK, GST_FLOW_ERROR
gst_dtcpip_chain(GstPad * pad, GstObject * parent, GstBuffer * inbuf) {
    gint ret_val;
    GstDtcpIp *filter;
    GstMapInfo map;
    gchar* encrypted_data;
    gchar* cleartext_data;
    size_t encrypted_size, cleartext_size;
    GstBuffer *outbuf;
    GstFlowReturn gfr = GST_FLOW_ERROR;

    filter = GST_DTCPIP (parent);
    gst_buffer_map(inbuf, &map, GST_MAP_READ);
    GST_LOG_OBJECT(filter, "input buffer %p, %zu bytes", map.data, map.size);

    // 1. set our encrypted data pointer
    encrypted_data = (gchar*) map.data;
    encrypted_size = map.size;

    // 2. Call the DTCPIP decryption
    if (!filter->dtcp_disabled) {
        ret_val = g_dtcpip_ftable->dtcpip_snk_alloc_decrypt(
                filter->session_handle, encrypted_data, encrypted_size,
                &cleartext_data, &cleartext_size);
        if (IS_DTCPIP_FAILURE(ret_val)) {
            GST_ERROR_OBJECT(filter,
                    "Failed decrypting: dtcpip_snk_alloc_decrypt(), ret_val=%d",
                    ret_val);
            return GST_FLOW_ERROR;
        }
    }

    // 3. Create a newly allocated buffer (refcount=1) without any data
    if (!filter->dtcp_disabled) {
        outbuf = gst_buffer_new_and_alloc(cleartext_size);
    }

    // 4. Set the new buffer's data to be the decrypted data
    if (!filter->dtcp_disabled) {
        gst_buffer_fill(outbuf, 0, (guint8*) cleartext_data, cleartext_size);
        gst_buffer_map(outbuf, &map, GST_MAP_READ);
        GST_LOG_OBJECT(filter, "output buffer %p, %zu bytes", map.data,
                map.size);
    }

    // 5. push the data to our sink pad, and onto the downstream element
    if (!filter->dtcp_disabled) {
        gfr = gst_pad_push(filter->srcpad, outbuf);
        if (gfr != GST_FLOW_OK) {
            GST_LOG_OBJECT(filter, "Failure with flow, ret_val=%d", gfr);
        }
    } else {
        // Not doing any encryption so just push in buffer through
        gfr = gst_pad_push(filter->srcpad, inbuf);
        if (gfr != GST_FLOW_OK) {
            GST_LOG_OBJECT(filter, "Failure with flow, ret_val=%d", gfr);
        }
    }

#ifdef DEBUG_SAVE_BUFFER_CONTENT
    if (fwrite(map.data, map.size, 1, g_debugBufferFile) != 1)
    {
        GST_WARNING_OBJECT(filter, "Failed to write %u bytes to debug file", cleartext_size);
    }
#endif

    // 6. Free the cleartext buffer that was allocated implicitly
    if (!filter->dtcp_disabled) {
        ret_val = g_dtcpip_ftable->dtcpip_snk_free(cleartext_data);
        if (IS_DTCPIP_FAILURE(ret_val)) {
            GST_ERROR_OBJECT(filter,
                    "Failure calling dtcpip_snk_free(), ret_val=%d", ret_val);
        }
    }

    return gfr;
}
*/
static GstFlowReturn
gst_dtcpip_transform(GstBaseTransform *trans, GstBuffer * inbuf, GstBuffer * outbuf) {
    GstDtcpIp *filter;
    gint ret_val;
    GstMapInfo map;
    gchar* encrypted_data;
    gchar* cleartext_data;
    size_t encrypted_size, cleartext_size;
    GstFlowReturn gfr = GST_FLOW_OK;
    filter = GST_DTCPIP (trans);

    gst_buffer_map(inbuf, &map, GST_MAP_READ);
    GST_LOG_OBJECT(filter, "input buffer %p, %zu bytes", map.data, map.size);

    // 1. set our encrypted data pointer
    encrypted_data = (gchar*) map.data;
    encrypted_size = map.size;

    // 2. Call the DTCPIP decryption
    if (!filter->dtcp_disabled) {
        ret_val = g_dtcpip_ftable->dtcpip_snk_alloc_decrypt(
                filter->session_handle, encrypted_data, encrypted_size,
                &cleartext_data, &cleartext_size);
        if (IS_DTCPIP_FAILURE(ret_val)) {
            GST_ERROR_OBJECT(filter,
                    "Failed decrypting: dtcpip_snk_alloc_decrypt(), ret_val=%d",
                    ret_val);
            return GST_FLOW_ERROR;
        }
    }

    // 3. Create a newly allocated buffer (refcount=1) without any data
    //if (!filter->dtcp_disabled) {
    //    outbuf = gst_buffer_new_and_alloc(cleartext_size);
    //}

    // 4. Set the new buffer's data to be the decrypted data
    if (!filter->dtcp_disabled) {
        gst_buffer_fill(outbuf, 0, (guint8*) cleartext_data, cleartext_size);
        gst_buffer_map(outbuf, &map, GST_MAP_READ);
        GST_LOG_OBJECT(filter, "output buffer %p, %zu bytes", map.data,
                map.size);
    }

    // 5. push the data to our sink pad, and onto the downstream element
    //if (!filter->dtcp_disabled) {
    //    gfr = gst_pad_push(filter->srcpad, outbuf);
    //    if (gfr != GST_FLOW_OK) {
    //        GST_LOG_OBJECT(filter, "Failure with flow, ret_val=%d", gfr);
    //    }
    //} else {
        // Not doing any encryption so just push in buffer through
    //    gfr = gst_pad_push(filter->srcpad, inbuf);
    //    if (gfr != GST_FLOW_OK) {
    //        GST_LOG_OBJECT(filter, "Failure with flow, ret_val=%d", gfr);
    //    }
    //}

#ifdef DEBUG_SAVE_BUFFER_CONTENT
    if (fwrite(map.data, map.size, 1, g_debugBufferFile) != 1)
    {
        GST_WARNING_OBJECT(filter, "Failed to write %u bytes to debug file", cleartext_size);
    }
#endif

    // 6. Free the cleartext buffer that was allocated implicitly
    if (!filter->dtcp_disabled) {
        ret_val = g_dtcpip_ftable->dtcpip_snk_free(cleartext_data);
        if (IS_DTCPIP_FAILURE(ret_val)) {
            GST_ERROR_OBJECT(filter,
                    "Failure calling dtcpip_snk_free(), ret_val=%d", ret_val);
            gfr = GST_FLOW_ERROR;
        }
    }

    return gfr;
}

/* given @caps on the src or sink pad (given by @direction)
 * calculate the possible caps on the other pad.
 *
 * Returns new caps, unref after usage.
 */
static GstCaps *
gst_dtcpip_transform_caps (GstBaseTransform * trans,
    GstPadDirection direction, GstCaps * caps, GstCaps * filter)
{
  GstCaps *ret;
  GstStructure* caps_struct;
  //gint caps_struct_cnt;
  //int i;
  GstDtcpIp* dtcpip = GST_DTCPIP (trans);

  GST_INFO_OBJECT (trans, "Called with caps: %" GST_PTR_FORMAT, caps);
  if (direction == GST_PAD_SRC) {
      // For source pad, return caps on sink pad which will always be "application/x-dtcp1
      GST_INFO_OBJECT (trans, "Source pad direction, should return x-dtcp");
      if (filter) {
        // If a filter was supplied, Need to return an intersection b/w caps and filter supplied
        ret = gst_caps_intersect_full (filter, caps, GST_CAPS_INTERSECT_FIRST);
        GST_INFO_OBJECT (trans, "returning intersection of caps & filter");
      } else {
        ret = gst_caps_ref (caps);
        GST_INFO_OBJECT (trans, "No filter was supplied so just returning caps");
      }

      // Make sure returning caps are what we expect which is x-dtcp1 & extract server & port
      if (gst_caps_get_size(ret) == 1) {
          caps_struct = gst_caps_get_structure(ret, 0);
          GST_INFO_OBJECT(trans, "Got ret struct %" GST_PTR_FORMAT, caps_struct);
          if (gst_structure_get_value(caps_struct, "DTCP1HOST") != NULL) {
            dtcpip->dtcp1host = g_value_dup_string(gst_structure_get_value(caps_struct, "DTCP1HOST"));
            GST_INFO_OBJECT(trans, "Set dtcp host to: %s", dtcpip->dtcp1host);
          } else
            GST_INFO_OBJECT(trans, "Has no field DTCP1HOST");
          if (gst_structure_get_value(caps_struct, "DTCP1PORT") != NULL) {
            dtcpip->dtcp1port = g_value_get_int(gst_structure_get_value(caps_struct, "DTCP1PORT"));
            GST_INFO_OBJECT(trans, "Set dtcp port to: %d", dtcpip->dtcp1port);
          } else
            GST_INFO_OBJECT(trans, "Has no field DTCP1PORT");
      } else {
          GST_INFO_OBJECT(trans, "Ret caps had unexpect struct cnt: %d %" GST_PTR_FORMAT,
                  gst_caps_get_size(ret), caps);
      }

      if (gst_caps_is_any(ret)) {
          GST_INFO_OBJECT(trans, "Forcing return of x-dtcp1");
          ret = gst_caps_new_empty_simple ("application/x-dtcp1");
      } else
          GST_INFO_OBJECT(trans, "Returning x-dtcp1");

  } else if (direction == GST_PAD_SINK) {
      GST_INFO_OBJECT (trans, "sink pad direction");
      // For sink pad, return caps on source pad which will always be "any"
      ret = gst_caps_new_any();

  } else {
      GST_ERROR_OBJECT (trans, "Returning NULL due to unsupported direction: %d", direction);
      ret = NULL;
  }

  GST_INFO_OBJECT (trans, "returning caps: %" GST_PTR_FORMAT, ret);
  return ret;
}


/* entry point to initialize the plug-in
 * initialize the plug-in itself
 * register the element factories and other features
 */
static gboolean dtcpip_init(GstPlugin * dtcpip) {
    /* debug category for fltering log messages
     *
     * exchange the string 'Template ' with your description
     */
    g_printf("dtcpip_init() called\n");
    GST_DEBUG_CATEGORY_INIT(gst_dtcpip_debug, "dtcpip", 0,
            "DTCP-IP library diagnostic output");

    return gst_element_register(dtcpip, "dtcpip", GST_RANK_PRIMARY,
            GST_TYPE_DTCPIP);
}

/* gstreamer looks for this structure to register the dtcpip plugin
 *
 * exchange the string 'Template dtcpip' with your dtcpip description
 */
GST_PLUGIN_DEFINE (
        GST_VERSION_MAJOR,
        GST_VERSION_MINOR,
        dtcpip,
        "DTCP (Digital Transmission Content Protection) plugin",
        (GstPluginInitFunc)dtcpip_init,
        VERSION,
        "LGPL",
        "CableLabs RUI-H RI",
        "http://www.cablelabs.com/"
)
