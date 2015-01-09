/// @file pathest/exponential_smoothing.h
/// @brief Class for exponential smoothing.
//===----------------------------------------------------------------------===//

#ifndef PATHEST_EXPONENTIAL_SMOOTHING_H_
#define PATHEST_EXPONENTIAL_SMOOTHING_H_

#include "pathest/location.h"

namespace pathest {

class ExponentialSmoothing {
 public:
  explicit ExponentialSmoothing(const double smoothing);
  ~ExponentialSmoothing() {}

  /// Predict the next location in chronological order.
  Location predict(const Location &loc);

 private:
  bool first_;  //< Whether or not the first data point has been processed yet.
  double pred_x_;  //< Last-predicted x coordinate.
  double pred_y_;  //< Last-predicted y coordinate.
  double smoothing_;  //< Smoothing factor.
  double smooth(const double meas, const double pred);  //< Apply smoothing.
};

}  // namespace pathest

#endif  // PATHEST_EXPONENTIAL_SMOOTHING_H_
