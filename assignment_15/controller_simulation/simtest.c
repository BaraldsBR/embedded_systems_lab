#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "spi.h"
#include "time.h"
#include "controller.h"
#include "printtypes.h"

#define SPEED 1000000
#define BYTES 4
#define PI 3.14159

#define PITCH_ENCODER_STEPS 1000
#define YAW_ENCODER_STEPS 1000

#define PITCH_PWM_STEPS 2500
#define YAW_PWM_STEPS 2500

int main(int argc, char *argv[])
{
  int step;
  int spiDevice;

  spi_content_t pwm_out;
  spi_content_t enc_in;
  
  pwm_out.pitch = 0;
  pwm_out.yaw = 0;

  controller_input controller_in;

  controller_in.pitch_current_position = 500 * 2 * PI / PITCH_ENCODER_STEPS;
  controller_in.yaw_current_position = -500 * 2 * PI / YAW_ENCODER_STEPS;

  controller_in.pitch_target_position = - PI / 4;
  controller_in.yaw_target_position = PI / 4;

  controller_output controller_out;

  long time_loop_start;
  long time_loop_spi;
  long time_loop_end;
  long spi_usec;
  long elapsed_usec;

  spiDevice = spiOpen(1, speed, 0);
  if (spiDevice < 0) return 2; /* <- very important do not remove!!!!*/

  controller_out = startController(controller_in);

  printf("start time;end time;spi time;elapsed utime\n");
  
  for (step = 0; step < 10000; step++) {
    time_loop_start = time_time();

    spiXfer(spiDevice, SPEED, (void*)&pwm_out, (void*)&enc_in, BYTES);
    time_loop_spi = time_time();
    
    controller_in.pitch_current_position = 500 * 2 * PI / PITCH_ENCODER_STEPS;
    controller_in.yaw_current_position = -500 * 2 * PI / YAW_ENCODER_STEPS;

    controller_out = advanceController(controller_in);

    pwm_out.pitch = controller_out.pitch_out * PITCH_PWM_STEPS;
    pwm_out.yaw = controller_out.yaw_out * YAW_PWM_STEPS;

    time_loop_end = time_time();

    elapsed_usec = time_loop_end - time_loop_start;
    
    printf("%ld;%ld;%ld;%ld\n",
      time_loop_start % 1000000000,
      time_loop_end % 1000000000,
      spi_usec,
      elapsed_usec);

    precise_sleep(10000 - elapsed_usec);
  }

  return 0;
}