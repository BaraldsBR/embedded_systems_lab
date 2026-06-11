#include <fcntl.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "spi.h"
#include "time.h"
#include "controller.h"

#define SPEED 1000000
#define BYTES 4
#define PI 3.14159

#define PITCH_ENCODER_STEPS 8000
#define YAW_ENCODER_STEPS 6000

#define PITCH_PWM_STEPS 2500
#define YAW_PWM_STEPS 2500

int main(int argc, char *argv[])
{
  int speed = SPEED;
  int spiDevice;

  spi_content_t pwm_out;
  spi_content_t enc_in;
  
  pwm_out.pitch = 0;
  pwm_out.yaw = 0;

  controller_input controller_in;

  controller_in.pitch_current_position = 0;
  controller_in.yaw_current_position = 0;

  controller_in.pitch_target_position = - PI / 4;
  controller_in.yaw_target_position = PI / 4;

  controller_output controller_out;

  long time_loop_start;
  long time_loop_end;
  long elapsed_usec;


  if (argc > 1) speed = atoi(argv[1]);
  if ((speed < 32000) || (speed > 250000000)) speed = SPEED;

  spiDevice = spiOpen(1, speed, 0);
  if (spiDevice < 0) return 2; /* <- very important do not remove!!!!*/

  controller_out = startController(controller_in);

  for (;;) {
    time_loop_start = time_time();
    spiXfer(spiDevice, speed, (void*)&pwm_out, (void*)&enc_in, BYTES);
    
    controller_in.pitch_current_position = enc_in.pitch * 2 * PI / PITCH_ENCODER_STEPS;
    controller_in.yaw_current_position = enc_in.yaw * 2 * PI / YAW_ENCODER_STEPS;

    controller_out = advanceController(controller_in);

    pwm_out.pitch = controller_out.pitch_out * PITCH_PWM_STEPS;
    pwm_out.yaw = controller_out.yaw_out * YAW_PWM_STEPS;

    time_loop_end = time_time();

    elapsed_usec = time_loop_end - time_loop_start;
    precise_sleep(10000 - elapsed_usec);
  }

  spiClose(spiDevice);

  return 0;
}