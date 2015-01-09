/// @file pathest/path.h
/// @brief Class for storing path data.
///
/// Path objects are expected to have at least one or two locations when running
/// certain public functions. These functions may have undefined behavior when
/// these expectations are not followed.
///
//===----------------------------------------------------------------------===//

#ifndef PATHEST_PATH_H_
#define PATHEST_PATH_H_

#include <stddef.h>
#include <utility>
#include <vector>

#include "pathest/location.h"

namespace pathest {

class Path {
 public:
  Path();
  explicit Path(std::vector<Location> locations);
  ~Path() {}

  typedef std::vector<Location>::iterator iterator;
  typedef std::vector<Location>::const_iterator const_iterator;
  iterator begin();
  iterator end();
  const_iterator begin() const;
  const_iterator end() const;

  bool empty() const;
  size_t size() const;

  /// @brief Get the minimum x coordinate.
  ///
  /// Undefined behavior for paths with zero locations.
  double min_x() const;

  /// @brief Get the minimum y coordinate.
  ///
  /// Undefined behavior for paths with zero locations.
  double min_y() const;

  /// @brief Get the minimum timestamp.
  ///
  /// Undefined behavior for paths with zero locations.
  double min_t() const;

  /// @brief Get the maximum x coordinate.
  ///
  /// Undefined behavior for paths with zero locations.
  double max_x() const;

  /// @brief Get the maximum y coordinate.
  ///
  /// Undefined behavior for paths with zero locations.
  double max_y() const;

  /// @brief Get the maximum timestamp.
  ///
  /// Undefined behavior for paths with zero locations.
  double max_t() const;

  /// @brief Insert a location from coordinates.
  ///
  /// @param x The x coordinate.
  /// @param y The y coordinate.
  /// @param t The timestamp.
  void insert(const double x, const double y, const double t);

  void insert(const Location &loc);  //< Insert a location object.

  /// @defgroup Query
  ///
  /// Functions for estimation of location for a given time.
  ///
  /// @{

  /// @brief Predict location given a time with a simple moving average.
  ///
  /// Assumes the following:
  /// - The number of samples is greater than zero.
  /// - The path has at least two locations.
  ///
  /// This function has undefined behavior when the assumptions do not hold.
  ///
  /// @param samples The number of samples factored into each average.
  /// @param time The given time.
  /// @param returns a coordinate pair representing location.
  std::pair<double, double> sma_predict(const int samples,
                                        const double time) const;

  /// @brief Predict location given a time with exponential smoothing.
  ///
  /// Assumes the following:
  /// - The smoothing factor is greater than zero and less than one.
  /// - The path has at least two locations.
  ///
  /// This function has undefined behavior when the assumptions do not hold.
  ///
  /// @param smoothing The smoothing factor.
  /// @param time The given time.
  /// @param returns a coordinate pair representing location.
  std::pair<double, double> es_predict(const double smoothing,
                                       const double time) const;

  /// @brief Predict location given a time with a kalman filter.
  ///
  /// Assumes the path has at least two locations. This function has undefined
  /// behavior when the assumption does not hold.
  ///
  /// @param time The given time.
  /// @param returns a coordinate pair representing location.
  std::pair<double, double> kf_predict(const double time) const;

  /// @}

  /// @defgroup Playback
  ///
  /// Functions for producing overall path of movement through time.
  ///
  /// @{

  /// @brief Calculate an estimated path with a simple moving average.
  ///
  /// Assumes the number of samples is greater than zero. If this assumption is
  /// broken this function returns an empty path. Otherwise, the estimated data
  /// will have an equal number of points as the input data.
  ///
  /// @param samples The number of samples factored into each average.
  /// @returns the estimated path if successful, an empty path otherwise.
  Path sma_path(const int samples) const;

  /// @brief Calculate an estimated path with exponential smoothing.
  ///
  /// Assumes the smoothing factor is greater than zero and less than one. If
  /// this assumption is broken this function returns an empty path. Otherwise,
  /// the estimated data will have an equal number of points as the input data.
  ///
  /// @param smoothing The smoothing factor.
  /// @returns the estimated path if successful, an empty path otherwise.
  Path es_path(const double smoothing) const;

  /// @brief Calculate an estimated path with a Kalman filter.
  ///
  /// The estimated data has an equal number of data points as the input data.
  ///
  /// @returns The estimated data.
  Path kf_path() const;

  /// @}

  /// @defgroup Info
  ///
  /// Function for estimating speed of movement along the path.
  ///
  /// @{

  /// @brief Calculate average speed.
  ///
  /// Estimates the speed of movement by taking the average of the estimated
  /// speed between each point. Undefined behavior for paths with fewer than two
  /// locations.
  double avg_speed() const;

  /// @}

 private:
  std::vector<Location> data_;  //< List of locations.
  std::pair<double, double> predict(const double time) const;
};

}  // namespace pathest

#endif  // PATHEST_PATH_H_
