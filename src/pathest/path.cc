/// @file pathest/path.cc
/// @brief Class for storing path data.
/// @bug Needs documentation and better error reporting.
//===----------------------------------------------------------------------===//

#include "pathest/path.h"

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <algorithm>
#include <vector>

#include "pathest/exponential_smoothing.h"
#include "pathest/kalman_filter.h"
#include "pathest/location.h"
#include "pathest/simple_moving_average.h"

namespace pathest {

Path::Path() : data_(std::vector<Location>()) {}

bool Path::empty() const { return this->data_.empty(); }
size_t Path::size() const { return this->data_.size(); }

Path::iterator Path::begin() { return this->data_.begin(); }
Path::iterator Path::end() { return this->data_.end(); }

Path::const_iterator Path::begin() const { return this->data_.begin(); }
Path::const_iterator Path::end() const { return this->data_.end(); }

void Path::insert(const Location &loc) {
  this->data_.insert(std::upper_bound(this->begin(), this->end(), loc,
                                      Location::comp_t), loc);
}

void Path::insert(double x, double y, double t) {
  this->insert(Location(x, y, t));
}

double Path::min_x() const {
#ifdef DEBUG
  assert(!this->empty());
#endif
  if (this->empty()) return 0;
  return (*std::min_element(this->begin(),
                            this->end(), Location::comp_x)).x();
}

double Path::min_y() const {
#ifdef DEBUG
  assert(!this->empty());
#endif
  if (this->empty()) return 0;
  return (*std::min_element(this->begin(), this->end(), Location::comp_y)).y();
}

double Path::min_t() const {
#ifdef DEBUG
  assert(!this->empty());
#endif
  if (this->empty()) return 0;
  return (*std::min_element(this->begin(), this->end(), Location::comp_t)).t();
}

double Path::max_x() const {
#ifdef DEBUG
  assert(!this->empty());
#endif
  if (this->empty()) return 0;
  return (*std::max_element(this->begin(), this->end(), Location::comp_x)).x();
}

double Path::max_y() const {
#ifdef DEBUG
  assert(!this->empty());
#endif
  if (this->empty()) return 0;
  return (*std::max_element(this->begin(), this->end(), Location::comp_y)).y();
}

double Path::max_t() const {
#ifdef DEBUG
  assert(!this->empty());
#endif
  if (this->empty()) return 0;
  return (*std::max_element(this->begin(), this->end(), Location::comp_t)).t();
}

double Path::avg_speed() const {
#ifdef DEBUG
  assert(this->size() > 1);
#endif
  if (this->size() < 2) return 0;
  double sum = 0.0;
  double num = 0.0;
  Path::const_iterator it = this->begin();
  double prev_x = it->x();
  double prev_y = it->y();
  double prev_t = it->t();
  for (Path::const_iterator it = this->begin(); it != this->end(); ++it) {
    double curr_x = it->x();
    double curr_y = it->y();
    double curr_t = it->t();
    if (curr_t != prev_t) {
      double dx = curr_x - prev_x;
      double dy = curr_y - prev_y;
      double dt = curr_t - prev_t;
      if (dx * dx + dy * dy >= 0) {
        double speed = sqrt(dx * dx + dy * dy) / dt;
        sum += speed;
        ++num;
      }
    }
  }
  if (!num) return 0;
  return sum / num;
}

Path Path::simple_moving_average(const int samples) const {
  Path data;
  if (samples <= 0) return data;
  SimpleMovingAverage sma(samples);
  for (Path::const_iterator it = this->begin(); it != this->end(); ++it) {
    data.insert(sma.predict(*it));
  }
  return data;
}

Path Path::exponential_smoothing(const double smoothing) const {
  Path data;
  if ((smoothing <= 0) || (smoothing >= 1.0)) return data;
  ExponentialSmoothing es(smoothing);
  for (Path::const_iterator it = this->begin(); it != this->end(); ++it) {
    data.insert(es.predict(*it));
  }
  return data;
}

Path Path::kalman_filter() const {
  Path data;
  KalmanFilter kf;
  for (Path::const_iterator it = this->begin(); it != this->end(); ++it) {
    data.insert(kf.predict(*it));
  }
  return data;
}

}  // namespace pathest
