/* appsrc + stream addapted mainly from:
 * - https://superuser.com/questions/1462693/how-to-stream-screen-to-remote-computer-with-gstreamer
 * - https://github.com/agrechnev/gst_app_tutorial/blob/master/video3.cpp
 */
#include "loop.h"

#include <gst/gst.h>
#include <gst/app/gstappsrc.h>
#include <glib.h>
#include <stdio.h>
#include <stdbool.h>
#include <pthread.h>

#include "../constants.h"
#include "image-processing.h"
#include "linMap.h"

#include "../controller/loop.h"

#if STREAM_IMAGE == 1
static GstElement *stream_pipeline, *stream_source;
static bool send_data = true, stream_started = false;
#endif

static GstFlowReturn new_sample (GstElement *sink) {
  GstSample *sample;
  GstBuffer *buffer_in;
  yuyv_packet_t *packed_image;
  uint32_t vertical_center = 0, horizontal_center = 0, mass = 0;

  double curr_pitch = controller_in.pitch_current_position;
  double curr_yaw = controller_in.yaw_current_position;

  printf("\n");
  printf("Current pitch,yaw: %f, %f\n", curr_pitch, curr_yaw);

  /* Retrieve the buffer */
  g_signal_emit_by_name (sink, "pull-sample", &sample);

  if (sample) {
    printf("New frame received\n");
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
#if STREAM_IMAGE == 1
      int total_cols = IMAGE_WIDTH / 2;
      for (int i = vertical_center - 4; i <= vertical_center + 4 && i < IMAGE_HEIGHT; i++) {
        for (int j = (horizontal_center - 4)/2; j <= (horizontal_center + 4)/2 && j < total_cols; j++) {
          int curr_packet = i * total_cols + j;
          if (i == vertical_center - 4 || i == vertical_center + 4 || j == (horizontal_center - 4)/2 || j == (horizontal_center + 4)/2) {
            packed_image[curr_packet].Y1 = (uint8_t) 128;
            packed_image[curr_packet].Y2 = (uint8_t) 128;
            packed_image[curr_packet].U = (uint8_t) 0;
            packed_image[curr_packet].V = (uint8_t) 255;
          } else {
            packed_image[curr_packet].Y1 = (uint8_t) 128;
            packed_image[curr_packet].Y2 = (uint8_t) 128;
            packed_image[curr_packet].U = (uint8_t) 255;
            packed_image[curr_packet].V = (uint8_t) 0;
          }
        }
      }
#endif
      
      printf("Low green\n");
    } else {
      vertical_center = vertical_center / mass;
      horizontal_center = horizontal_center / mass;
    }

    printf("Mass: %u pixels (%.4f)\n", mass, (double)mass/(IMAGE_HEIGHT*IMAGE_WIDTH));
    printf("Ball pos (ver,hor): %u, %u pixel\n", vertical_center, horizontal_center);

    double pitch_diff = PI/180 * linMap(vertical_center, 
                                        0, IMAGE_HEIGHT, 
                                        FOV_V/2, -FOV_V/2);
                      
    double yaw_diff   = PI/180 * linMap(horizontal_center, 
                                        0, IMAGE_WIDTH, 
                                        FOV_H/2, -FOV_H/2);

    printf("Pos diff (pitch,yaw): %.4f, %.4f rad\n", pitch_diff, yaw_diff);

    double new_target_pitch = curr_pitch + pitch_diff;
    double new_target_yaw = curr_yaw + yaw_diff;

    printf("Target pos (pitch,yaw): %.4f, %.4f rad\n", new_target_pitch, new_target_yaw);

    controller_in.pitch_target_position = new_target_pitch;
    controller_in.yaw_target_position = new_target_yaw;

#if STREAM_IMAGE == 1
    if (send_data) {
      if (!stream_started) {
        stream_started = true;
        g_print ("Now streaming:\n");
        gst_element_set_state (stream_pipeline, GST_STATE_PLAYING);
      }

      size_t buffer_size = IMAGE_HEIGHT * IMAGE_WIDTH * 2;

      GstBuffer *buffer_out = gst_buffer_new_and_alloc(buffer_size);
      GstMapInfo map_out;
      
      gst_buffer_map(buffer_out, &map_out, GST_MAP_WRITE);
      memcpy(map_out.data, packed_image, buffer_size);
      gst_buffer_unmap(buffer_out, &map_out);
      buffer_out->pts = buffer_in->pts;

      gst_app_src_push_buffer(GST_APP_SRC(stream_source), buffer_out);
    }
#endif

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

void* imageProcessingLoop (void* args)
{
  GMainLoop *loop;

  GstElement *process_pipeline, *source, *sink;
  GstBus *process_bus;
  guint process_bus_watch_id;
  
  // stream_pipeline and stream_source are static so they can accesed from the new_sample function
  GstElement *stream_jpegenc, *stream_rtpenc, *stream_sink;
  GstBus *stream_bus;
  guint stream_bus_watch_id;

  GstCaps *caps;

  /* Initialisation */
  gst_init (NULL, NULL);

  loop = g_main_loop_new (NULL, FALSE);


  /* Define capabilities */
  caps = gst_caps_new_simple ("video/x-raw",
                              "format", G_TYPE_STRING, "YUY2",
                              "width", G_TYPE_INT, IMAGE_WIDTH,
                              "height", G_TYPE_INT, IMAGE_HEIGHT,
                              "framerate", GST_TYPE_FRACTION, VIDEO_FPS, 1,
                              NULL);

  /* Create gstreamer elements */
  /* Image Processing */
  process_pipeline   = gst_pipeline_new ("");

  source      = gst_element_factory_make ("v4l2src",       "webcam-source");
  sink        = gst_element_factory_make ("appsink",       "app-sink");

  if (!process_pipeline || !source || !sink) {
    g_printerr ("One processing element could not be created. Exiting.\n");
    return NULL;
  }

  /* Set up the process_pipeline */

  g_object_set (G_OBJECT (source), "device", DEVICE_NAME, NULL);
  g_object_set (G_OBJECT (sink), "emit-signals", TRUE, NULL);
  g_signal_connect (G_OBJECT (sink), "new-sample", G_CALLBACK(new_sample), NULL);
  
  /* we add message handlers */
  process_bus = gst_pipeline_get_bus (GST_PIPELINE (process_pipeline));
  process_bus_watch_id = gst_bus_add_watch (process_bus, bus_call, loop);
  gst_object_unref (process_bus);


  /* we add all elements into the pipeline */
  gst_bin_add_many (GST_BIN (process_pipeline),
                    source, sink, NULL);

  gst_element_link_filtered (source, sink, caps);
  
#if STREAM_IMAGE == 1
  /* Repeat for Streaming */
  stream_pipeline    = gst_pipeline_new ("");
  stream_source  = gst_element_factory_make ("appsrc",     "stream-source");
  stream_jpegenc = gst_element_factory_make ("jpegenc",    "stream-jpegenc");
  stream_rtpenc  = gst_element_factory_make ("rtpjpegpay", "stream-rtpenc");
  stream_sink    = gst_element_factory_make ("udpsink",    "stream-sink");
  
  if (!stream_pipeline) {
    g_printerr ("stream_pipeline could not be created. Exiting.\n");
    return NULL;
  }
    if (!stream_source) {
    g_printerr ("stream_source element could not be created. Exiting.\n");
    return NULL;
  }
    if (!stream_jpegenc) {
    g_printerr ("stream_jpegenc element could not be created. Exiting.\n");
    return NULL;
  }
    if (!stream_rtpenc) {
    g_printerr ("stream_rtpenc element could not be created. Exiting.\n");
    return NULL;
  }
    if (!stream_sink) {
    g_printerr ("stream_sink element could not be created. Exiting.\n");
    return NULL;
  }
    
  g_object_set (G_OBJECT (stream_sink), "host", STREAM_IP, NULL);
  g_object_set (G_OBJECT (stream_sink), "port", STREAM_PORT, NULL);
  
  stream_bus = gst_pipeline_get_bus (GST_PIPELINE (stream_pipeline));
  stream_bus_watch_id = gst_bus_add_watch (stream_bus, bus_call, loop);
  gst_object_unref (stream_bus);

  gst_bin_add_many (GST_BIN (stream_pipeline),
                  stream_source, stream_jpegenc,
                  stream_rtpenc, stream_sink, NULL);

  gst_element_link_filtered (stream_source, stream_jpegenc, caps);
  gst_element_link (stream_jpegenc, stream_rtpenc);
  gst_element_link (stream_rtpenc, stream_sink);
#endif

  gst_caps_unref(caps);

  /* Set the process_pipeline to "playing" state*/
  g_print ("Now recording:\n");
  gst_element_set_state (process_pipeline, GST_STATE_PLAYING);


  /* Iterate */
  g_print ("Running...\n");
  g_main_loop_run (loop);

  /* Out of the main loop, clean up nicely */
  
  g_print ("Deleting process_pipeline\n");
  
  gst_element_set_state (process_pipeline, GST_STATE_NULL);
  
  gst_object_unref (GST_OBJECT (process_pipeline));
  g_source_remove (process_bus_watch_id);
  g_main_loop_unref (loop);
  
#if STREAM_IMAGE == 1
  g_print ("Deleting stream_pipeline\n");
  
  gst_element_set_state (stream_pipeline, GST_STATE_NULL);
  
  gst_object_unref (GST_OBJECT (stream_pipeline));
  g_source_remove (stream_bus_watch_id);
  g_main_loop_unref (loop);
#endif

  return NULL;
}
