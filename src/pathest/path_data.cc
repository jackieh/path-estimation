/// @file pathest/path_data.cc
/// @brief Classes for storing location data.
/// @bug Needs documentation.
//===----------------------------------------------------------------------===//

#include "pathest/path_data.h"

#include <assert.h>
#include <math.h>
#include <stddef.h>
#include <stdio.h>
#include <algorithm>
#include <vector>

namespace pathest {

Location::Location(double x, double y, double t) : x_(x), y_(y), t_(t) {}

bool Location::comp_x(const Location &loc1, const Location &loc2) {
  return loc1.x() < loc2.x();
}

bool Location::comp_y(const Location &loc1, const Location &loc2) {
  return loc1.y() < loc2.y();
}

bool Location::comp_t(const Location &loc1, const Location &loc2) {
  return loc1.t() < loc2.t();
}

double Location::x() const { return this->x_; }
double Location::y() const { return this->y_; }
double Location::t() const { return this->t_; }

PathData::PathData() : data_(std::vector<Location>()) {}

bool PathData::empty() const { return this->data_.empty(); }

size_t PathData::size() const { return this->data_.size(); }

PathData::iterator PathData::begin() { return this->data_.begin(); }
PathData::iterator PathData::end() { return this->data_.end(); }

PathData::const_iterator PathData::begin() const { return this->data_.begin(); }
PathData::const_iterator PathData::end() const { return this->data_.end(); }

void PathData::add_point(double x, double y, double t) {
  this->data_.insert(std::upper_bound(this->data_.begin(), this->data_.end(),
                                      Location(x, y, t), Location::comp_t),
                     Location(x, y, t));
}

double PathData::min_x() const {
#ifdef DEBUG
  assert(!this->empty());
#endif
  if (this->empty()) return 0;
  return (*std::min_element(this->begin(),
                            this->end(), Location::comp_x)).x();
}

double PathData::min_y() const {
#ifdef DEBUG
  assert(!this->empty());
#endif
  if (this->empty()) return 0;
  return (*std::min_element(this->begin(), this->end(), Location::comp_y)).y();
}

double PathData::min_t() const {
#ifdef DEBUG
  assert(!this->empty());
#endif
  if (this->empty()) return 0;
  return (*std::min_element(this->begin(), this->end(), Location::comp_t)).t();
}

double PathData::max_x() const {
#ifdef DEBUG
  assert(!this->empty());
#endif
  if (this->empty()) return 0;
  return (*std::max_element(this->begin(), this->end(), Location::comp_x)).x();
}

double PathData::max_y() const {
#ifdef DEBUG
  assert(!this->empty());
#endif
  if (this->empty()) return 0;
  return (*std::max_element(this->begin(), this->end(), Location::comp_y)).y();
}

double PathData::max_t() const {
#ifdef DEBUG
  assert(!this->empty());
#endif
  if (this->empty()) return 0;
  return (*std::max_element(this->begin(), this->end(), Location::comp_t)).t();
}

double PathData::avg_speed() const {
#ifdef DEBUG
  assert(this->size() > 1);
#endif
  if (this->size() < 2) return 0;
  double sum = 0.0;
  double num = 0.0;
  PathData::const_iterator it = this->begin();
  double prev_x = it->x();
  double prev_y = it->y();
  double prev_t = it->t();
  for (PathData::const_iterator it = this->begin(); it != this->end(); ++it) {
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
