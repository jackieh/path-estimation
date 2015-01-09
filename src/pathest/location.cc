/// @file location.cc
/// @brief Class for storing location data.
//===----------------------------------------------------------------------===//

#include "pathest/location.h"

namespace pathest {

Location::Location(const double x, const double y, const double t) :
  x_(x), y_(y), t_(t) {}

bool Location::comp_x(const Location &loc1, const Location &loc2) {
  return loc1.x_ < loc2.x_;
}

bool Location::comp_y(const Location &loc1, const Location &loc2) {
  return loc1.y_ < loc2.y_;
}

bool Location::comp_t(const Location &loc1, const Location &loc2) {
  return loc1.t_ < loc2.t_;
}

double Location::x() const { return this->x_; }
double Location::y() const { return this->y_; }
double Location::t() const { return this->t_; }

}  // namespace pathest
