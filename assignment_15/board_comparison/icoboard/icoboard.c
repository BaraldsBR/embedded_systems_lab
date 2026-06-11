/*
 * spi-pigpio-speed.c
 * 2016-11-23
 * Public Domain
*/

#include <sys/time.h> 
#include <fcntl.h>
#include <getopt.h>
#include <linux/spi/spidev.h>
#include <linux/types.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <unistd.h>

/*
   gcc -pthread -o spi-pigpio-speed spi-pigpio-speed.c -lpigpio
   sudo ./spi-pigpio-speed [bytes [bps [loops] ] ]
*/

#define SPEED 10000000
#define BYTES 4

double time_time(void) {
  struct timeval tv;
  double t;

  gettimeofday(&tv, 0);

  t = (double)tv.tv_sec + ((double)tv.tv_usec / 1E6);

  return t;
}


int spiOpen(unsigned spiChan, unsigned spiBaud, unsigned spiFlags) {
  int i, fd;
  char spiMode;
  char spiBits = 8;
  char dev[32];

  spiMode = spiFlags & 3;
  spiBits = 8;

  sprintf(dev, "/dev/spidev0.%d", spiChan);

  if ((fd = open(dev, O_RDWR)) < 0) {
    return -1;
  }

  if (ioctl(fd, SPI_IOC_WR_MODE, &spiMode) < 0) {
    close(fd);
    return -2;
  }

  if (ioctl(fd, SPI_IOC_WR_BITS_PER_WORD, &spiBits) < 0) {
    close(fd);
    return -3;
  }

  if (ioctl(fd, SPI_IOC_WR_MAX_SPEED_HZ, &spiBaud) < 0) {
    close(fd);
    return -4;
  }

  return fd;
}

int spiClose(int fd) { return close(fd); }

int spiRead(int fd, unsigned speed, char *buf, unsigned count) {
  int err;
  struct spi_ioc_transfer spi;

  memset(&spi, 0, sizeof(spi));

  spi.tx_buf = (unsigned long)NULL;
  spi.rx_buf = (unsigned long)buf;
  spi.len = count;
  spi.speed_hz = speed;
  spi.delay_usecs = 0;
  spi.bits_per_word = 8;
  spi.cs_change = 0;

  err = ioctl(fd, SPI_IOC_MESSAGE(1), &spi);

  return err;
}

int spiWrite(int fd, unsigned speed, void *buf, unsigned count) {
  int err;
  struct spi_ioc_transfer spi;

  memset(&spi, 0, sizeof(spi));

  spi.tx_buf = (unsigned long)buf;
  spi.rx_buf = (unsigned long)NULL;
  spi.len = count;
  spi.speed_hz = speed;
  spi.delay_usecs = 0;
  spi.bits_per_word = 8;
  spi.cs_change = 0;

  err = ioctl(fd, SPI_IOC_MESSAGE(1), &spi);

  return err;
}

int spiXfer(int fd, unsigned speed, void *txBuf, void *rxBuf, unsigned count) {
  int err;
  struct spi_ioc_transfer spi;

  memset(&spi, 0, sizeof(spi));

  spi.tx_buf = (unsigned long)txBuf;
  spi.rx_buf = (unsigned long)rxBuf;
  spi.len = count;
  spi.speed_hz = speed;
  spi.delay_usecs = 0;
  spi.bits_per_word = 8;
  spi.cs_change = 0;

  err = ioctl(fd, SPI_IOC_MESSAGE(1), &spi);

  return err;
}

typedef struct _spi_content_t {
  int16_t pitch;
  int16_t yaw;
} spi_content_t;

int main(int argc, char *argv[])
{
    int speed = SPEED;
    int spiDevice;

    int counter = 123;
    int response;

    double start_time;
    double end_time = 0;
    /* usage: sudo ./spi-pigpio-speed [bps] */
    if (argc > 1) speed = atoi(argv[1]);
    if ((speed < 32000) || (speed > 250000000)) speed = SPEED;

    spiDevice = spiOpen(1, speed, 0);
    if (spiDevice < 0) return 2;

    spiXfer(spiDevice, speed, (void*)&counter, (void*)&response, BYTES);

    start_time = time_time();

    spiXfer(spiDevice, speed, (void*)&counter, (void*)&response, BYTES);

    end_time = time_time();

    printf("Single transfer, read value: %d, total time: %f \n", response, end_time - start_time);

    // counter = 0;

    // start_time = time_time();

    // while(counter < 1000000) {
    //   spiXfer(spiDevice, speed, (void*)&counter, (void*)&response, BYTES);
    //   counter++;
    // }

    // end_time = time_time();

    // printf("counter: %d, read value: %d, total time: %f \n", counter, response, end_time - start_time);
    

    counter = 0;

    spi_content_t test_out;

    test_out.pitch = 0x0102;  
    test_out.yaw   = 0x0304;    

    spiXfer(spiDevice, speed, (void*)&test_out, (void*)&response, BYTES);
    spiXfer(spiDevice, speed, (void*)&test_out, (void*)&response, BYTES);

    // while(counter < 100) {
    //   spiXfer(spiDevice, speed, (void*)&counter, (void*)&response, BYTES);
    //   if ((counter - 1) != response) printf("value mismatch\n");
    //   counter++;
    // }

    spiClose(spiDevice);
    return 0;
}