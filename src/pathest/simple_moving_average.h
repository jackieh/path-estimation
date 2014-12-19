/// @file pathest/simple_moving_average.h
/// @brief Class for simple moving average.
//===----------------------------------------------------------------------===//

#ifndef PATHEST_SIMPLE_MOVING_AVERAGE_H_
#define PATHEST_SIMPLE_MOVING_AVERAGE_H_

#include <stddef.h>
#include <vector>

#include "pathest/location.h"

namespace pathest {

class SimpleMovingAverage {
 public:
  explicit SimpleMovingAverage(const size_t samples);
  ~SimpleMovingAverage() {}

  /// Predict the next location in chronological order.
  Location predict(const Location &loc);

 private:
  double sum_x_;  //< Sum of x coordinates.
  double sum_y_;  //< Sum of y coordinates.
  std::vector<double> history_x_;  //< History of x coordinates.
  std::vector<double> history_y_;  //< History of y coordinates.
  size_t num_predicted_;  //< Number of samples in history that are predicted.
  size_t history_index_;  //< Index into the history buffers.
  const size_t samples_;  //< Number of samples factored into each average.
};

}  // namespace pathest

#endif  // PATHEST_SIMPLE_MOVING_AVERAGE_H_
