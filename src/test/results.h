/// @file results.h
/// @brief Class for writing data analysis results.
/// @bug Needs more documentation.
//===----------------------------------------------------------------------===//

#ifndef TEST_RESULTS_H_
#define TEST_RESULTS_H_

#include <string>

#include "pathest/path.h"

class Results {
 public:
  Results(const char *, const pathest::Path &);

  void add_reference(const pathest::Path &);
  void write(const char *, const char *, const pathest::Path &) const;

 private:
  std::string out_dir_;
  pathest::Path ref_data_;

  double x_min_;
  double x_max_;
  double y_min_;
  double y_max_;

  void init_report() const;
  void write_json(const char *, const pathest::Path &) const;
  void write_error(const char *, const pathest::Path &) const;
  void write_plot(const char *, const char *, const pathest::Path &) const;
  double mean_absolute_error(const pathest::Path &) const;
  double root_mean_square_error(const pathest::Path &) const;
  double mean_absolute_scaled_error(const pathest::Path &) const;
};

#endif  // TEST_RESULTS_H_
