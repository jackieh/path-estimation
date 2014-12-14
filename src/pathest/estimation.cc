/// @file pathest/estimation.cc
/// @brief Functions for filtering noise from path data.
/// @bug Needs refactoring, more functionality, and improved error reporting.
//===----------------------------------------------------------------------===//

#include "pathest/estimation.h"

#include <assert.h>
#include <stdio.h>
#include <armadillo>
#include <utility>

#include "pathest/path_data.h"

namespace pathest {

PathData simple_moving_average(const int samples, const PathData &input) {
#ifdef DEBUG
  // Invariant: We take the average of a positive number of samples.
  assert(samples > 0);
#endif
  std::pair<double, double> sum(0.0, 0.0);  // Sum of previous measurements.
  std::pair<double, double> buf[samples];  // Buffer for measurement history.
  int num = 0;  // Number of samples factored into the sum.
  int idx = 0;  // Index into the measurement buffer.
  PathData data;

  for (PathData::const_iterator it = input.begin(); it != input.end(); ++it) {
#ifdef DEBUG
    assert(num <= samples);  // Invariant: num does not exceed samples.
    assert(idx < samples);  // Invariant: no invalid array access possible.
#endif
    double x = it->x();
    double y = it->y();
    double t = it->t();
    if (num < samples) {
      ++num;
    } else {
      sum.first -= buf[idx].first;
      sum.second -= buf[idx].second;
    }
    sum.first += x;
    sum.second += y;
    buf[idx].first = x;
    buf[idx].second = y;
    idx = (idx + 1) % samples;
#ifdef DEBUG
    assert(num > 0);  // Invariant: no division by zero possible.
#endif
    data.add_point(sum.first / num, sum.second / num, t);
  }

  return data;
}

PathData exp_moving_average(const double smoothing, const PathData &input) {
#ifdef DEBUG
  // Invariant: We assume 0 < smoothing < 1.
  assert(smoothing > 0);
  assert(smoothing < 1.0);
#endif
  double x_est, y_est;  // Current coordinate pair estimate.
  bool first = true;  // Whether we have analyzed the first estimate.
  PathData data;

  for (PathData::const_iterator it = input.begin(); it != input.end(); ++it) {
    double x = it->x();
    double y = it->y();
    double t = it->t();
    if (first) {
      first = false;
      x_est = x;
      y_est = y;
    } else {
      x_est = smoothing * x + (1 - smoothing) * x_est;
      y_est = smoothing * y + (1 - smoothing) * y_est;
    }
    data.add_point(x_est, y_est, t);
  }

  return data;
}

PathData kalman_filter(const PathData &input) {
  PathData data;

  // Constant matrices.
  const arma::mat I = arma::eye(4, 4);
  arma::mat A, H, Q, R;
  A.eye(4, 4);
  A(0, 2) = A(1, 3) = 0.2;
  H.zeros(4, 4);
  H(0, 0) = H(0, 2) = H(1, 1) = H(1, 3) = 1.0;
  Q.zeros(4, 4);
  Q(2, 2) = Q(3, 3) = 0.1;
  R = 0.1 * arma::eye(4, 4);

  // State matrices.
  arma::colvec m, x, y;
  arma::mat K, P, S;
  m = x = arma::zeros(4, 1);
  P = arma::zeros(4, 4);

  for (PathData::const_iterator it = input.begin(); it != input.end(); ++it) {
    m(0, 0) = it->x();
    m(1, 0) = it->y();

    // Prediction step.
    x = A * x;
    P = (A * P * A.t()) + Q;

    // Update step.
    S = (H * P * H.t()) + R;
    K = P * H.t() * solve(S, I);
    y = m - (H * x);
    x = x + (K * y);
    P = (I - (K * H)) * P;

    data.add_point(x(0, 0), x(1, 0), it->t());
  }

  return data;
}

}  // namespace pathest
