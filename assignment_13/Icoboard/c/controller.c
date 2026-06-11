#include "controller.h"
#include "controller/xxsubmod.h"

XXDouble inputs [4 + 1];
XXDouble outputs [2 + 1];

controller_output startController(controller_input input) {

  /* Initialize the inputs and outputs with correct initial values */
  inputs[0] = input.pitch_current_position;  /* pitch_current_position */
  inputs[1] = input.pitch_target_position;   /* pitch_target_position */
  inputs[2] = input.yaw_current_position;    /* yaw_current_position */
  inputs[3] = input.yaw_target_position;     /* yaw_target_position */

  outputs[0] = 0.0;    /* pitch_out */
  outputs[1] = 0.0;    /* yaw_out */

  /* Initialize the submodel itself */
  XXInitializeSubmodel (inputs, outputs, xx_time);

  controller_output out;

  out.pitch = outputs[0];
  out.yaw = outputs[1];

  return out;
}

controller_output advanceController(controller_input input) {
  inputs[0] = input.pitch_current_position;  /* pitch_current_position */
  inputs[1] = input.pitch_target_position;   /* pitch_target_position */
  inputs[2] = input.yaw_current_position;    /* yaw_current_position */
  inputs[3] = input.yaw_target_position;     /* yaw_target_position */

  XXCalculateSubmodel (inputs, outputs, xx_time);
    
  controller_output out;

  out.pitch = outputs[0];
  out.yaw = outputs[1];

  return out;
}

