/// @file results.h
/// @brief Class for writing data analysis results.
/// @bug Needs more documentation.
//===----------------------------------------------------------------------===//

#ifndef TEST_RESULTS_H_
#define TEST_RESULTS_H_

#include "pathest/path_data.h"

class Results {
 public:
  Results(const char *, const pathest::PathData &);

  void add_reference(const pathest::PathData &);
  void write(const char *, const char *, const pathest::PathData &) const;

 private:
  const char *output_dir;
  pathest::PathData reference_data;

  double x_min_;
  double x_max_;
  double y_min_;
  double y_max_;

  void init_report() const;
  void write_json(const char *, const pathest::PathData &) const;
  void write_error(const char *, const pathest::PathData &) const;
  void write_plot(const char *, const char *, const pathest::PathData &) const;
  double mean_absolute_error(const pathest::PathData &) const;
  double root_mean_square_error(const pathest::PathData &) const;
  double mean_absolute_scaled_error(const pathest::PathData &) const;
};

#endif  // TEST_RESULTS_H_
