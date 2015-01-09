/// @file pathest/path.cc
/// @brief Class for storing path data.
///
/// Path objects maintain the invariant that its vector of location objects is
/// always sorted with respect to each location's timestamp.
///
/// @bug Query functions are lacking testing.
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
Path::Path(std::vector<Location> locations) : data_(locations) {
  std::sort(this->data_.begin(), this->data_.end(), Location::comp_t);
}

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

void Path::insert(const double x, const double y, const double t) {
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

std::pair<double, double> Path::predict(const double time) const {
#ifdef DEBUG
  assert(this->size() > 1);
#endif
  if (this->size() < 2) return std::pair<double, double>(0, 0);
  double speed = this->avg_speed();
  if (time < this->min_t()) {
    // Case 1: time < min_t
    double dt = this->data_[0].t() - time;
    double angle = atan2(this->data_[1].x() - this->data_[0].x(),
                  this->data_[1].y() - this->data_[0].y());
    double dx = dt * speed * cos(angle);
    double dy = dt * speed * sin(angle);
    return std::pair<double, double>(data_[0].x() - dx, data_[0].y() - dy);
  } else if (time > this->max_t()) {
    // Case 2: time < max_t
    int len = this->data_.size();
    double dt = time - this->data_[len - 1].t();
    double angle = atan2(this->data_[len - 2].x() - this->data_[len - 1].x(),
                  this->data_[len - 2].y() - this->data_[len - 1].y());
    double dx = dt * speed * cos(angle);
    double dy = dt * speed * sin(angle);
    return std::pair<double, double>(data_[len - 1].x() - dx,
                                     data_[len - 1].y() - dy);
  } else {
    // Case 3: min_t <= time <= max_t
    for (Path::const_iterator it = this->begin(); it != this->end(); ++it) {
      if (time < it->t()) {
#ifdef DEBUG
        assert(it - 1 != this->begin());
#endif
        double avg_x = ((it - 1)->x() + it->x()) / 2.0;
        double avg_y = ((it - 1)->y() + it->y()) / 2.0;
        return std::pair<double, double>(avg_x, avg_y);
      }
    }
  }
#ifdef DEBUG
  assert(false);
#endif
  return std::pair<double, double>(0, 0);
}

std::pair<double, double> Path::sma_predict(const int samples,
                                            const double time) const {
#ifdef DEBUG
  assert(samples > 0);
  assert(this->size() > 1);
#endif
  if ((samples <= 0) || (this->size() < 2)) {
    return std::pair<double, double>(0, 0);
  } else {
    return this->sma_path(samples).predict(time);
  }
}

std::pair<double, double> Path::es_predict(const double smoothing,
                                           const double time) const {
#ifdef DEBUG
  assert((smoothing > 0) && (smoothing < 1.0));
  assert(this->size() > 1);
#endif
  if ((smoothing <= 0) || (smoothing >= 1.0) || (this->size() < 2)) {
    return std::pair<double, double>(0, 0);
  } else {
    return this->es_path(smoothing).predict(time);
  }
}

std::pair<double, double> Path::kf_predict(const double time) const {
#ifdef DEBUG
  assert(this->size() > 1);
#endif
  if (this->size() < 2) {
    return std::pair<double, double>(0, 0);
  } else {
    return this->kf_path().predict(time);
  }
}

Path Path::sma_path(const int samples) const {
  Path data;
  if (samples <= 0) return data;
  SimpleMovingAverage sma(samples);
  for (Path::const_iterator it = this->begin(); it != this->end(); ++it) {
    data.insert(sma.predict(*it));
  }
  return data;
}

Path Path::es_path(const double smoothing) const {
  Path data;
  if ((smoothing <= 0) || (smoothing >= 1.0)) return data;
  ExponentialSmoothing es(smoothing);
  for (Path::const_iterator it = this->begin(); it != this->end(); ++it) {
    data.insert(es.predict(*it));
  }
  return data;
}

Path Path::kf_path() const {
  Path data;
  KalmanFilter kf;
  for (Path::const_iterator it = this->begin(); it != this->end(); ++it) {
    data.insert(kf.predict(*it));
  }
  return data;
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
  for (; it != this->end(); ++it) {
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

}  // namespace pathest
