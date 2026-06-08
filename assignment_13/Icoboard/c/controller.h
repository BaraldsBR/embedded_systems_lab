#ifndef CONTROLLER
#define CONTROLLER

typedef struct _controller_input {
  double pitch_current_position;
  double pitch_target_position;
  double yaw_current_position;
  double yaw_target_position;
} controller_input;

typedef struct _controller_output {
  double pitch_out;
  double yaw_out;
} controller_output;

controller_output startController(controller_input input);
controller_output advanceController(controller_input input);

#endif