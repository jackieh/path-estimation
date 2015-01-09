/// @file pathest/exponential_smoothing.cc
/// @brief Class for simple moving average.
/// @bug May need better error reporting.
//===----------------------------------------------------------------------===//

#include "pathest/exponential_smoothing.h"

#include <stddef.h>
#include <assert.h>
#include <vector>

#include "pathest/location.h"

namespace pathest {

double ExponentialSmoothing::smooth(const double meas, const double pred) {
  return this->smoothing_ * meas + (1 - this->smoothing_) * pred;
}

ExponentialSmoothing::ExponentialSmoothing(const double smoothing) :
  first_(true),
  pred_x_(0),
  pred_y_(0),
  smoothing_(smoothing) {}

Location ExponentialSmoothing::predict(const Location &loc) {
  if ((this->smoothing_ <= 0) || (this->smoothing_ >= 1.0)) return loc;

  double x = loc.x();
  double y = loc.y();
  if (this->first_) {
    this->first_ = false;
    this->pred_x_ = x;
    this->pred_y_ = y;
  } else {
    this->pred_x_ = this->smooth(x, this->pred_x_);
    this->pred_y_ = this->smooth(y, this->pred_y_);
  }

  double pred_x = this->pred_x_;
  double pred_y = this->pred_y_;
  return Location(pred_x, pred_y, loc.t());
}

}  // namespace pathest
