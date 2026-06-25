#include "linMap.h"

double linMap (double x,
               double x1,
               double x2,
               double y1,
               double y2) {
  double m = (y2 - y1)/(x2 - x1);
  double c = y1 - m * x1;
  return m * x + c;
}