/// @file pathest/estimation.h
/// @brief Functions for filtering noise from path data.
/// @bug Needs refactoring and better interface design.
//===----------------------------------------------------------------------===//

#ifndef PATHEST_ESTIMATION_H_
#define PATHEST_ESTIMATION_H_

#include "pathest/path_data.h"

namespace pathest {

/// @brief Calculate a simple moving average.
///
/// The estimated data has an equal number of data points as the input data.
///
/// Assumes the number of samples is greater than zero. If this assumption is
/// broken this function has undefined behavior.
///
/// @param samples The number of samples factored into each average.
/// @param input The input data.
PathData simple_moving_average(const int samples, const PathData &input);

/// @brief Calculate a exponential moving average.
///
/// The estimated data has an equal number of data points as the input data.
///
/// Assumes the smoothing factor is greater than zero and less than one. If
/// this assumption is broken this function has undefined behavior.
///
/// @param smoothing The smoothing factor.
/// @param input The input data.
PathData exp_moving_average(const double smoothing, const PathData &input);

/// @brief Kalman filter estimate.
///
/// The estimated data has an equal number of data points as the input data.
///
/// @param input The input data.
/// @returns The estimated data.
PathData kalman_filter(const PathData &input);

}  // namespace pathest

#endif  // PATHEST_ESTIMATION_H_
