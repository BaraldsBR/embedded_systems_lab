#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "loop.h"

#include "spi.h"
#include "time.h"
#include "controller.h"
#include "../constants.h"

void calibrate_jiwy(int spiDevice, 
                    spi_content_t* pos_min, 
                    spi_content_t* pos_max) {

  spi_content_t pwm_out;
  spi_content_t pos_in;
  spi_content_t pos_in_old;

  // find minimum
  pos_in.pitch = 32767; // initialise unlikely values
  pos_in.yaw   = 32767; 

  pwm_out.pitch = -150;
  pwm_out.yaw   = -200;
  do {
    pos_in_old = pos_in;
    spiXfer(spiDevice, SPI_SPEED, (void*)&pwm_out, (void*)&pos_in, SPI_BYTES_PER_TRANSFER);
    precise_sleep(5000);
  } while (pos_in.pitch != pos_in_old.pitch || pos_in.yaw != pos_in_old.yaw);

  (*pos_min) = pos_in;

  // find maximum
  pos_in.pitch = 32767; // initialise unlikely values
  pos_in.yaw   = 32767; 

  pwm_out.pitch = 150;
  pwm_out.yaw   = 200;  
  do {
    pos_in_old = pos_in;
    spiXfer(spiDevice, SPI_SPEED, (void*)&pwm_out, (void*)&pos_in, SPI_BYTES_PER_TRANSFER);
    precise_sleep(5000);
  } while (pos_in.pitch != pos_in_old.pitch || pos_in.yaw != pos_in_old.yaw);

  (*pos_max) = pos_in;

  // turn off
  pwm_out.pitch = 0;
  pwm_out.yaw = 0;
  spiXfer(spiDevice, SPI_SPEED, (void*)&pwm_out, (void*)&pos_in, SPI_BYTES_PER_TRANSFER);

  return;
}

pos_rad pos2rad(spi_content_t pos, 
                spi_content_t pos_min, 
                spi_content_t pos_max) {
  pos_rad out;
  out.pitch = PI*14/11 * (pos.pitch - pos_min.pitch)/(pos_max.pitch - pos_min.pitch);
  out.yaw = PI * (pos.yaw - pos_min.yaw)/(pos_max.yaw - pos_min.yaw);
  return out;
}

controller_input controller_in = {0, 0, 0, 0};

void* controllerLoop(void* args)
{
  spi_content_t pwm_out = { 0, 0 };
  spi_content_t pos_in;
  pos_rad pos_in_rad;

  controller_output controller_out;

  controller_in.pitch_target_position = PI * 7/11;
  controller_in.yaw_target_position = PI / 2;

  long time_loop_start;
  long time_loop_spi;
  long time_loop_end;
  long elapsed_usec = 0;

  int spiDevice;
  spiDevice = spiOpen(1, SPI_SPEED, 0);
  if (spiDevice < 0) return NULL;

  spi_content_t pos_min;
  spi_content_t pos_max;

  calibrate_jiwy(spiDevice, &pos_min, &pos_max);

  startController(controller_in);

  for (;;) {
    time_loop_start = time_time();

    spiXfer(spiDevice, SPI_SPEED, (void*)&pwm_out, (void*)&pos_in, SPI_BYTES_PER_TRANSFER);
    
    time_loop_spi = time_time();
        
    printf("current spi usec: %ld\n", time_loop_spi - time_loop_start);
    printf("previous elapsed usec: %ld\n\n", elapsed_usec);

    pos_in_rad = pos2rad(pos_in, pos_min, pos_max);
    controller_in.pitch_current_position = pos_in_rad.pitch;
    controller_in.yaw_current_position = pos_in_rad.yaw;
    
    controller_out = advanceController(controller_in);
    
    pwm_out.pitch = controller_out.pitch * PITCH_PWM_STEPS;
    pwm_out.yaw = controller_out.yaw * YAW_PWM_STEPS;
    
    time_loop_end = time_time();
    elapsed_usec = time_loop_end - time_loop_start;
    precise_sleep(1000 - elapsed_usec);
  }

  spiClose(spiDevice);

  return NULL;
}