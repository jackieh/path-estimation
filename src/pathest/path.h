/// @file pathest/path.h
/// @brief Class for storing path data.
//===----------------------------------------------------------------------===//

#ifndef PATHEST_PATH_H_
#define PATHEST_PATH_H_

#include <stddef.h>
#include <vector>

#include "pathest/location.h"

namespace pathest {

class Path {
 public:
  Path();
  ~Path() {}

  typedef std::vector<Location>::iterator iterator;
  typedef std::vector<Location>::const_iterator const_iterator;
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  bool empty() const;
  size_t size() const;

  double min_x() const;  //< Returns the minimum x coordinate.
  double min_y() const;  //< Returns the minimum y coordinate.
  double min_t() const;  //< Returns the minimum timestamp.
  double max_x() const;  //< Returns the maximum x coordinate.
  double max_y() const;  //< Returns the maximum y coordinate.
  double max_t() const;  //< Returns the maximum timestamp.
  double avg_speed() const;  //< Calculate average speed.

  void insert(double x, double y, double t);
  void insert(const Location &loc);

  /// @brief Calculate an estimated path with a simple moving average.
  ///
  /// Assumes the number of samples is greater than zero. If this assumption is
  /// broken this function returns an empty path. Otherwise, the estimated data
  /// will have an equal number of points as the input data.
  ///
  /// @param samples The number of samples factored into each average.
  /// @returns the estimated path if successful, an empty path otherwise.
  Path simple_moving_average(const int samples) const;

  /// @brief Calculate an estimated path with exponential smoothing.
  ///
  /// Assumes the smoothing factor is greater than zero and less than one. If
  /// this assumption is broken this function returns an empty path. Otherwise,
  /// the estimated data will have an equal number of points as the input data.
  ///
  /// @param smoothing The smoothing factor.
  Path exponential_smoothing(const double smoothing) const;

  /// @brief Calculate an estimated path with a Kalman filter.
  ///
  /// The estimated data has an equal number of data points as the input data.
  ///
  /// @returns The estimated data.
  Path kalman_filter() const;

 private:
  std::vector<Location> data_;  //< List of locations.
};

}  // namespace pathest

#endif  // PATHEST_PATH_H_
