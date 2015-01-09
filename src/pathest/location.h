/// @file location.h
/// @brief Class for storing location data.
//===----------------------------------------------------------------------===//

#ifndef PATHEST_LOCATION_H_
#define PATHEST_LOCATION_H_

namespace pathest {

class Location {
 public:
  Location(const double x, const double y, const double t);
  ~Location() {}

  // Comparison functions.
  static bool comp_x(const Location &loc1, const Location &loc2);
  static bool comp_y(const Location &loc1, const Location &loc2);
  static bool comp_t(const Location &loc1, const Location &loc2);

  // Field access.
  double x() const;
  double y() const;
  double t() const;

 private:
  double x_;  //< The x coordinate.
  double y_;  //< The y coordinate.
  double t_;  //< Timestamp.
};

}  // namespace pathest

#endif  // PATHEST_LOCATION_H_
