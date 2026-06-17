/* appsrc + stream addapted mainly from:
 * - https://superuser.com/questions/1462693/how-to-stream-screen-to-remote-computer-with-gstreamer
 * - https://github.com/agrechnev/gst_app_tutorial/blob/master/video3.cpp
 */

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <glib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#include "loop.h"
#include "image-processing.h"

static GstElement* stream_source;
static bool send_data = true;

static GstFlowReturn new_sample (GstElement *sink) {
  printf("new_sample() ran\n");
  GstSample *sample;
  GstBuffer *buffer_in;
  yuyv_packet_t *packed_image;
  uint32_t vertical_center = 0, horizontal_center = 0, mass = 0;

  /* Retrieve the buffer */
  g_signal_emit_by_name (sink, "pull-sample", &sample);

  if (sample) {
    buffer_in = gst_sample_get_buffer (sample);
    packed_image = (yuyv_packet_t*) g_malloc(sizeof(yuyv_packet_t) * IMAGE_WIDTH * IMAGE_HEIGHT / 2);

    size_t buffer_size = IMAGE_WIDTH * IMAGE_HEIGHT * 2;
    gst_buffer_extract(buffer_in, 0, (void*)packed_image, buffer_size);

    pthread_t subthread[SUBTHREADS];
    thread_processing_request_t req[SUBTHREADS];
    thread_processing_response_t* res;
  
    for (int i = 0; i < SUBTHREADS; i++) {
      req[i].image = packed_image;
      req[i].starting_row = i * IMAGE_HEIGHT / SUBTHREADS;
      req[i].row_count = IMAGE_HEIGHT / SUBTHREADS;
      req[i].row_size = IMAGE_WIDTH;
      pthread_create(&subthread[i], NULL, processImageChunk, &req[i]);
    }
    
    for (int i = 0; i < SUBTHREADS; i++) {
      pthread_join(subthread[i], (void**)&res);
      mass += res->total_mass;
      vertical_center += res->total_vertical_sum;
      horizontal_center += res->total_horizontal_sum;
      free(res);
    }

    if (mass < IMAGE_WIDTH * IMAGE_HEIGHT / 100) {
      vertical_center = IMAGE_HEIGHT / 2;
      horizontal_center = IMAGE_WIDTH / 2;
      printf("low green\n");
    } else {
      vertical_center = vertical_center / mass;
      horizontal_center = horizontal_center / mass;
    }

    if (send_data) {
      size_t buffer_size = IMAGE_HEIGHT * IMAGE_WIDTH * 2;

      GstBuffer *buffer_out = gst_buffer_new_and_alloc(buffer_size);
      GstMapInfo map_out;
      
      gst_buffer_map(buffer_out, &map_out, GST_MAP_WRITE);
      memcpy(map_out.data, packed_image, buffer_size);
      gst_buffer_unmap(buffer_out, &map_out);
      buffer_out->pts = buffer_in->pts;

      gst_app_src_push_buffer(GST_APP_SRC(stream_source), buffer_out);
    }

    printf("horizontal center: %u, vertical center: %u", horizontal_center, vertical_center);
    gst_sample_unref (sample);
    return GST_FLOW_OK;
  }

  return GST_FLOW_FLUSHING;
}

static gboolean
bus_call (GstBus     *bus,
          GstMessage *msg,
          gpointer    data)
{
  GMainLoop *loop = (GMainLoop *) data;

  switch (GST_MESSAGE_TYPE (msg)) {

    case GST_MESSAGE_EOS:
      g_print ("End of stream\n");
      g_main_loop_quit (loop);
      break;

    case GST_MESSAGE_ERROR: {
      gchar  *debug;
      GError *error;

      gst_message_parse_error (msg, &error, &debug);
      g_free (debug);

      g_printerr ("Error: %s\n", error->message);
      g_error_free (error);

      g_main_loop_quit (loop);
      break;
    }
    default:
      break;
  }

  return TRUE;
}

/// Callback called when the pipeline wants more data
void startFeed(GstElement *source, guint size) {
  send_data = true;
}

/// Callback called when the pipeline wants no more data for now
void stopFeed(GstElement *source) {
  send_data = false;
}

void* imageProcessingLoop (void* args)
{
  GMainLoop *loop;

  GstElement *pipeline, *source, *sink;
  // stream_source is static so it can accesed from the new_sample function
  GstElement *stream_jpegenc, *stream_rtpenc, *stream_sink;
  GstCaps *caps;
  GstBus *bus;
  guint bus_watch_id;

  // /* Initialisation */
  gst_init (NULL, NULL);

  loop = g_main_loop_new (NULL, FALSE);
  
  // /* Check input arguments */

  /* Create gstreamer elements */
  pipeline   = gst_pipeline_new ("");

  /* Image Processing */
  source      = gst_element_factory_make ("v4l2src",       "webcam-source");
  sink        = gst_element_factory_make ("appsink",       "app-sink");
  // /* Streaming */
  stream_source  = gst_element_factory_make ("appsrc",     "stream-source");
  stream_jpegenc = gst_element_factory_make ("jpegenc",    "stream-jpegenc");
  stream_rtpenc  = gst_element_factory_make ("rtpjpegpay", "stream-rtpenc");
  stream_sink    = gst_element_factory_make ("udpsink",    "stream-sink");

  if (!pipeline || !source || !sink) {
    g_printerr ("One element could not be created. Exiting.\n");
    return NULL;
  }

  /* Set up the pipeline */

  g_object_set (G_OBJECT (source), "device", DEVICE_NAME, NULL);
  g_object_set (G_OBJECT (sink), "emit-signals", TRUE, NULL);
  g_signal_connect (G_OBJECT (sink), "new-sample", G_CALLBACK(new_sample), NULL);

  g_object_set (G_OBJECT (stream_sink), "host", STREAM_IP, NULL);
  g_object_set (G_OBJECT (stream_sink), "port", STREAM_PORT, NULL);
  g_signal_connect(stream_source, "need-data", G_CALLBACK(startFeed), NULL);
  g_signal_connect(stream_source, "enough-data", G_CALLBACK(stopFeed), NULL);

  /* we add a message handler */
  bus = gst_pipeline_get_bus (GST_PIPELINE (pipeline));
  bus_watch_id = gst_bus_add_watch (bus, bus_call, loop);
  gst_object_unref (bus);

  /* Define capabilities */
  caps = gst_caps_new_simple ("video/x-raw",
                              "format", G_TYPE_STRING, "YUY2",
                              "width", G_TYPE_INT, IMAGE_WIDTH,
                              "height", G_TYPE_INT, IMAGE_HEIGHT,
                              "framerate", GST_TYPE_FRACTION, VIDEO_FPS, 1,
                              NULL);

  /* we add all elements into the pipeline */
  gst_bin_add_many (GST_BIN (pipeline),
                    source, sink,
                    stream_source, stream_jpegenc,
                    stream_rtpenc, stream_sink, NULL);

  gst_element_link_filtered (source, sink, caps);
  
  gst_element_link_filtered (stream_source, stream_jpegenc, caps);
  gst_element_link (stream_jpegenc, stream_rtpenc);
  gst_element_link (stream_rtpenc, stream_sink);

  gst_caps_unref(caps);

  /* Set the pipeline to "playing" state*/
  g_print ("Now recording:\n");
  gst_element_set_state (pipeline, GST_STATE_PLAYING);


  /* Iterate */
  g_print ("Running...\n");
  g_main_loop_run (loop);

  /* Out of the main loop, clean up nicely */
  g_print ("Returned, stopped recording\n");
  gst_element_set_state (pipeline, GST_STATE_NULL);

  g_print ("Deleting pipeline\n");
  gst_object_unref (GST_OBJECT (pipeline));
  g_source_remove (bus_watch_id);
  g_main_loop_unref (loop);

  return NULL;
}
