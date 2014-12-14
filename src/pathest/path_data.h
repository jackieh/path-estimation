/// @file path_data.h
/// @brief Declarations for path data.
/// @bug Needs documentation.
//===----------------------------------------------------------------------===//

#ifndef PATHEST_PATH_DATA_H_
#define PATHEST_PATH_DATA_H_

#include <stddef.h>
#include <vector>

namespace pathest {

class Location {
 public:
  Location(double x, double y, double t);
  ~Location() {}

  static bool comp_x(const Location &l1, const Location &l2);
  static bool comp_y(const Location &l1, const Location &l2);
  static bool comp_t(const Location &l1, const Location &l2);

  double x() const;
  double y() const;
  double t() const;

 private:
  double x_;
  double y_;
  double t_;
};

class PathData {
 public:
  PathData();
  ~PathData() {}

  bool empty() const;
  size_t size() const;
  void add_point(double, double, double);

  typedef std::vector<Location>::iterator iterator;
  typedef std::vector<Location>::const_iterator const_iterator;

  iterator begin();
  iterator end();

  const_iterator begin() const;
  const_iterator end() const;

  double min_x() const;
  double min_y() const;
  double min_t() const;
  double max_x() const;
  double max_y() const;
  double max_t() const;
  double avg_speed() const;

 private:
  std::vector<Location> data_;
};

}  // namespace pathest

#endif  // PATHEST_PATH_DATA_H_
