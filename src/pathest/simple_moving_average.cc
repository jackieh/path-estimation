/// @file pathest/simple_moving_average.cc
/// @brief Class for simple moving average.
/// @bug May need better error reporting.
//===----------------------------------------------------------------------===//

#include "pathest/simple_moving_average.h"

#include <stddef.h>
#include <assert.h>
#include <vector>

#include "pathest/location.h"

namespace pathest {

SimpleMovingAverage::SimpleMovingAverage(const size_t samples) :
  sum_x_(0.0),
  sum_y_(0.0),
  history_x_(std::vector<double>(samples)),
  history_y_(std::vector<double>(samples)),
  num_predicted_(0),
  history_index_(0),
  samples_(samples) {}

Location SimpleMovingAverage::predict(const Location &loc) {
  if (!this->samples_) return loc;

#ifdef DEBUG
  assert(this->num_predicted_ <= this->samples_);
  assert(this->history_index_ < this->samples_);  // No invalid array access.
#endif

  double x = loc.x();
  double y = loc.y();

  if (this->num_predicted_ < this->samples_) {
    ++this->num_predicted_;
  } else {
    this->sum_x_ -= this->history_x_[this->history_index_];
    this->sum_y_ -= this->history_y_[this->history_index_];
  }
  this->sum_x_ += x;
  this->sum_y_ += y;
  this->history_x_[this->history_index_] = x;
  this->history_y_[this->history_index_] = y;
  this->history_index_ = (this->history_index_ + 1) % this->samples_;

#ifdef DEBUG
  assert(this->num_predicted_ > 0);  // No division by zero possible.
#endif

  double pred_x = this->sum_x_ / this->num_predicted_;
  double pred_y = this->sum_y_ / this->num_predicted_;
  return Location(pred_x, pred_y, loc.t());
}

}  // namespace pathest
