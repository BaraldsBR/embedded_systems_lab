#include "pixel2rad.h"

#include "../constants.h"

double pixel2rad (int location, int max, int fov) {

  double location_norm = (double)location/max + 0.5;
  double result = (location_norm*fov)*(PI/180);
  return result;
}