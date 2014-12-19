/// @file pathest/kalman_filter.h
/// @brief Class for Kalman filter.
//===----------------------------------------------------------------------===//

#ifndef PATHEST_KALMAN_FILTER_H_
#define PATHEST_KALMAN_FILTER_H_

#include <armadillo>

#include "pathest/location.h"

namespace pathest {

class KalmanFilter {
 public:
  KalmanFilter();
  ~KalmanFilter() {}

  /// Predict the next location in chronological order.
  Location predict(const Location &loc);

 private:
  static const arma::mat A_, H_, I_, Q_, R_;  //< Constant matrices.
  arma::colvec m_, x_, y_;  //< State vectors.
  arma::mat K_, P_, S_;  //< State matrices.
};

}  // namespace pathest

#endif  // PATHEST_KALMAN_FILTER_H_
