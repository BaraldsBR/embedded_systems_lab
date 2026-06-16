/* Shows lum values from webcam */

#include <gst/gst.h>
#include <glib.h>
#include <stdio.h>

#define IMAGE_WIDTH 640
#define IMAGE_HEIGHT 480
#define VIDEO_FPS 30

#define MIN_Y 50
#define MAX_Y 75
#define MIN_Cb 50
#define MAX_Cb 75
#define MIN_Cr 50
#define MAX_Cr 75

#define CAMERA_DEVICE /dev/video0

static GstFlowReturn new_sample (GstElement *sink) {
  GstSample *sample;
  GstBuffer *buffer;
  u_int8_t dest_Y[IMAGE_WIDTH * IMAGE_HEIGHT];
  u_int8_t dest_Cb[IMAGE_WIDTH * IMAGE_HEIGHT / 4];
  u_int8_t dest_Cr[IMAGE_WIDTH * IMAGE_HEIGHT / 4];
  u_int32_t vertical_center = 0, horizontal_center = 0, mass = 0;

  /* Retrieve the buffer */
  g_signal_emit_by_name (sink, "pull-sample", &sample);

  if (sample) {
    buffer = gst_sample_get_buffer (sample);

    size_t size_Y = IMAGE_WIDTH * IMAGE_HEIGHT;
    gst_buffer_extract(buffer, 0, (void*)dest_Y, size_Y);

    size_t size_Cb = IMAGE_WIDTH * IMAGE_HEIGHT / 4;
    size_t offset_Cb = IMAGE_WIDTH * IMAGE_HEIGHT;
    gst_buffer_extract(buffer, offset_Cb, (void*)dest_Cb, size_Cb);

    size_t size_Cr = IMAGE_WIDTH * IMAGE_HEIGHT / 4;
    size_t offset_Cr = IMAGE_WIDTH * IMAGE_HEIGHT * 5 / 4;
    gst_buffer_extract(buffer, offset_Cr, (void*)dest_Cr, size_Cr);

    for (size_t row = 0; row < IMAGE_HEIGHT; row++)
    {
      for (size_t col = 0; col < IMAGE_WIDTH; col++)
      {
        int pos_Y = row * IMAGE_WIDTH + col;
        int pos_C = ((row / 2) * (IMAGE_WIDTH / 2)) + (col / 2); 
        if (dest_Y[pos_Y] > MIN_Y
         && dest_Y[pos_Y] < MAX_Y
         && dest_Cb[pos_C] > MIN_Cb
         && dest_Cb[pos_C] < MAX_Cb
         && dest_Cr[pos_C] > MIN_Cr
         && dest_Cr[pos_C] < MAX_Cr
        ) {
          vertical_center += row;
          horizontal_center += col;
          mass++;
        }
      }
    }

    if (mass == 0) {
      vertical_center = IMAGE_HEIGHT / 2;
      horizontal_center = IMAGE_WIDTH / 2;  
    } else {
      vertical_center = vertical_center / mass;
      horizontal_center = horizontal_center / mass;
    }

    printf("horizontal center: %ld, vertical center: %ld", horizontal_center, vertical_center);
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


int main (int argc, char *argv[])
{
  GMainLoop *loop;

  GstElement *pipeline, *source, *sink;
  GstCaps *caps;
  GstBus *bus;
  guint bus_watch_id;

  // /* Initialisation */
  gst_init (&argc, &argv);

  loop = g_main_loop_new (NULL, FALSE);
  
  // /* Check input arguments */
  if (argc != 2) {
    g_printerr ("Usage: %s <Device name>\n", argv[0]);
    return -1;
  }

  /* Create gstreamer elements */
  pipeline   = gst_pipeline_new ("");
  source     = gst_element_factory_make ("v4l2src",       "webcam-source");
  sink       = gst_element_factory_make ("appsink",       "app-sink");

  if (!pipeline || !source || !sink) {
    g_printerr ("One element could not be created. Exiting.\n");
    return -1;
  }

  /* Set up the pipeline */

  /* we set the input and output filename to the source element */
  g_object_set (G_OBJECT (source), "device", argv[1], NULL);
  g_object_set (G_OBJECT (sink), "emit-signals", TRUE, NULL);
  g_signal_connect (G_OBJECT (sink), "new-sample", G_CALLBACK(new_sample), NULL);


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
  /* file-source | ogg-demuxer | vorbis-decoder | converter | alsa-output */
  gst_bin_add_many (GST_BIN (pipeline),
                    source, sink, NULL);

  gst_element_link_filtered (source, sink, caps);

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

  return 0;
}
