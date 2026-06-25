## make test avi
gst-launch-1.0 -v -e v4l2src device=/dev/video0 ! jpegenc ! image/jpeg,width=640,height=480,framerate=30/1 ! avimux ! filesink location=file.avi

## make test yuv file
gst-launch-1.0 -v -e v4l2src device=/dev/video0 ! jpegenc ! image/jpeg,width=640,height=480,framerate=30/1 ! jpegdec ! filesink location=file.yuv

gst-launch-1.0 -v -e autovideosrc ! jpegenc ! image/jpeg,width=640,height=480,framerate=30/1 ! jpegdec ! filesink location=file.yuv

## play yuv with VLC
/Applications/_downloaded/VLC.app/Contents/MacOS/VLC --rawvid-fps 30 --rawvid-width 640 --rawvid-height 480 --rawvid-chroma I420 file.yuv

## play vid
gst-launch-1.0 -v -e autovideosrc ! jpegenc ! image/jpeg,width=640,height=480,framerate=30/1 ! jpegdec ! autovideosink

## Compile
gcc -Wall main.c -o main $(pkg-config --cflags --libs gstreamer-1.0)

## list caps of all camera devices
gst-device-monitor-1.0

## test streaming
gst-launch-1.0 -v autovideosrc ! jpegenc ! "image/jpeg,width=640, height=480,framerate=30/1" ! rtpjpegpay ! udpsink host=172.20.10.13  port=5001

gst-launch-1.0 -e -v udpsrc port=5001 ! application/x-rtp, encoding-name=JPEG,payload=26 ! rtpjpegdepay ! jpegdec ! autovideosink

## compiling for Mac OS
export PKG_CONFIG_PATH=/Library/Frameworks/GStreamer.framework/Versions/1.0/lib/pkgconfig
export PATH=/Library/Frameworks/GStreamer.framework/Versions/1.0/bin:$PATH

## Compiling 
gcc $(find . -name '*.c') -lm $(pkg-config --cflags --libs gstreamer-1.0 gstreamer-app-1.0)