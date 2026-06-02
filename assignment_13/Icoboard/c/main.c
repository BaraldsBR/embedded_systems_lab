/*
 * spi-pigpio-speed.c
 * 2016-11-23
 * Public Domain
*/


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
#include "printtypes.h"

/*
   gcc -pthread -o spi-pigpio-speed spi-pigpio-speed.c -lpigpio
   sudo ./spi-pigpio-speed [bytes [bps [loops] ] ]
*/

#define SPEED 1000000
#define BYTES 4


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

  spi.tx_buf = (unsigned)NULL;
  spi.rx_buf = (unsigned)buf;
  spi.len = count;
  spi.speed_hz = speed;
  spi.delay_usecs = 0;
  spi.bits_per_word = 8;
  spi.cs_change = 0;

  err = ioctl(fd, SPI_IOC_MESSAGE(1), &spi);

  return err;
}

int spiWrite(int fd, unsigned speed, char *buf, unsigned count) {
  int err;
  struct spi_ioc_transfer spi;

  memset(&spi, 0, sizeof(spi));

  spi.tx_buf = (unsigned)buf;
  spi.rx_buf = (unsigned)NULL;
  spi.len = count;
  spi.speed_hz = speed;
  spi.delay_usecs = 0;
  spi.bits_per_word = 8;
  spi.cs_change = 0;

  err = ioctl(fd, SPI_IOC_MESSAGE(1), &spi);

  return err;
}

int spiXfer(int fd, unsigned speed, char *txBuf, char *rxBuf, unsigned count) {
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

    spi_content_t pwm_out;
    spi_content_t enc_in;


    /* usage: sudo ./spi-pigpio-speed [bps] */
    if (argc > 1) speed = atoi(argv[1]);
    if ((speed < 32000) || (speed > 250000000)) speed = SPEED;

    spiDevice = spiOpen(1, speed, 0);
    if (spiDevice < 0) return 2;

    printf("Set PWM pitch yaw: ");
    scanf("%hd %hd", &pwm_out.pitch, &pwm_out.yaw);

    spiXfer(spiDevice, speed, (char)pwm_out, (char)enc_in, BYTES);

    printf("Read encoder (pitch,yaw) = (%hd,%hd)", enc_in.pitch, enc_out.yaw);

    spiClose(spiDevice);

    return 0;
}