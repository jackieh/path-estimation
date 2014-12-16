/// @file results.cc
/// @brief Class for writing data analysis results.
/// @bug Lacking testing.
//===----------------------------------------------------------------------===//

#include "test/results.h"

#include <assert.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <string>
#include <vector>

#include "json/json.h"
#include "pathest/path_data.h"
#include "plplot/plplot.h"
#include "plplot/plstream.h"

// PLplot constants.
#define WHITE 15           // Plot environment color white.
#define JUST 1             // The scales of x and y axes are made equal.
#define AXIS 0             // Draw box, ticks, and numeric tick labels.
#define NCOL1 256          // Number of colors allocated to the color palette.
#define ITYPE false        // Point colors defined with HLS.
#define NPTS 2             // Point colors defined with two control points.
#define ALT_HUE_PATH NULL  // No alternative interpolation when defining colors.
#define DOT 1              // Plot points as dots.

// PLplot constant arrays (HLS properties for each control point).
const double intensity[2] = {0.0, 1.0};
const double hue[2] = {240.0, 0.0};
const double light[2] = {0.6, 0.6};
const double sat[2] = {0.8, 0.8};

// Note: no effect if '/' is appended to a directory ending in '/' (POSIX).
const char *report_name = "report";
const char *txt_path_fmt = "%s/%s.txt";
const char *svg_path_fmt = "%s/%s.svg";
const size_t txt_path_len = strlen(txt_path_fmt) + 1 - 4;
const size_t svg_path_len = strlen(svg_path_fmt) + 1 - 4;

Results::Results(const char *dir, const pathest::PathData &input) :
  out_dir_(std::string(dir, strlen(dir))),
  ref_data_(pathest::PathData()),
  x_min_(input.min_x()), x_max_(input.max_x()),
  y_min_(input.min_y()), y_max_(input.max_y()) {
  this->init_report();
}

void Results::add_reference(const pathest::PathData &ref) {
#ifdef DEBUG
  // Invariant: do not set reference to more than one data set.
  assert(this->ref_data_.empty());
#endif
  if (ref.empty()) fprintf(stderr, "Warning: using empty reference data\n");
  for (pathest::PathData::const_iterator it = ref.begin(); it != ref.end();
       ++it) {
    this->ref_data_.add_point(it->x(), it->y(), it->t());
  }
  this->write("reference", "Reference data", this->ref_data_);
}

void Results::write(const char *name, const char *title,
                    const pathest::PathData &output) const {
  this->write_json(name, output);
  this->write_error(title, output);
  this->write_plot(name, title, output);
}

void Results::write_plot(const char *name, const char *title,
                         const pathest::PathData &output) const {
#ifdef DEBUG
  // Invariant: no invalid parameters.
  assert(name != NULL);
  assert(title != NULL);
#endif

  size_t plot_path_len = svg_path_len + this->out_dir_.length() + strlen(name);
  std::vector<char> plot_path(plot_path_len);
  snprintf(&plot_path[0], plot_path_len, svg_path_fmt, this->out_dir_.c_str(),
           name);
  plsdev("svg");
  plsfnam(&plot_path[0]);
  plinit();
  plcol0(WHITE);
  plenv(this->x_min_, this->x_max_, this->y_min_, this->y_max_, JUST, AXIS);
  plmtex("t", 2.0, 0.5, 0.5, title);
  plscmap1n(NCOL1);
  plscmap1l(ITYPE, NPTS, intensity, hue, light, sat, ALT_HUE_PATH);
  double t_min = output.min_t();
  double t_max = output.max_t();
  if (t_max != t_min) {
    for (pathest::PathData::const_iterator it = output.begin();
         it != output.end(); ++it) {
      double x = it->x();
      double y = it->y();
      double t = it->t();
#ifdef DEBUG
      // Invariant: no division by zero possible.
      assert((t_max - t_min) != 0);
#endif
      plcol1((t - t_min) / (t_max - t_min));
      plpoin(1, &x, &y, DOT);
    }
  }
  plend();
  fprintf(stdout, "Wrote plot to %s\n", &plot_path[0]);
}

void Results::write_json(const char *name, const pathest::PathData &output)
  const {
#ifdef DEBUG
  // Invariant: no invalid parameters.
  assert(name != NULL);
#endif
  size_t json_path_len = txt_path_len + this->out_dir_.length() + strlen(name);
  std::vector<char> json_path(json_path_len);
  snprintf(&json_path[0], json_path_len, txt_path_fmt, this->out_dir_.c_str(),
           name);

  Json::Value reports = Json::Value(Json::arrayValue);
  for (pathest::PathData::const_iterator it = output.begin();
       it != output.end(); ++it) {
    Json::Value location;
    location["x"] = it->x();
    location["y"] = it->y();
    location["timestamp"] = it->t();
    reports.append(location);
  }

  Json::Value root;
  root["target"] = "trains";
  root["reports"] = reports;

  FILE *fp = fopen(&json_path[0], "w");
  if (fp) {
    Json::StyledWriter writer;
    fprintf(fp, writer.write(root).c_str());
    fclose(fp);
    fprintf(stdout, "Wrote data to %s\n", &json_path[0]);
  } else {
    fprintf(stderr, "Warning: unable to open file: %s\n", &json_path[0]);
  }
}

void Results::write_error(const char *title, const pathest::PathData &output)
  const {
#ifdef DEBUG
  // Invariant: no invalid parameters.
  assert(title != NULL);
  // Invariant: output has the same number of data points as reference.
  if (!this->ref_data_.empty()) {
    assert(this->ref_data_.size() == output.size());
  }
#endif
  size_t report_path_len = txt_path_len + this->out_dir_.length()
    + strlen(report_name);
  std::vector<char> report_path(report_path_len);
  snprintf(&report_path[0], report_path_len, txt_path_fmt,
           this->out_dir_.c_str(), report_name);
  FILE *fp = fopen(&report_path[0], "a");
  if (fp) {
    fprintf(fp, "\n%s\n", title);
    if (!this->ref_data_.empty()) {
      fprintf(fp, "MAE: %f\n", this->mean_absolute_error(output));
      fprintf(fp, "RMSE: %f\n", this->root_mean_square_error(output));
      fprintf(fp, "MASE: %f\n", this->mean_absolute_scaled_error(output));
    }
    fprintf(fp, "Estimated speed: %f KPH\n", 60 * output.avg_speed());
    fclose(fp);
  } else {
    fprintf(stderr, "Warning: unable to open file: %s\n", &report_path[0]);
  }
}

void Results::init_report() const {
  size_t report_path_len = txt_path_len + this->out_dir_.length()
    + strlen(report_name);
  std::vector<char> report_path(report_path_len);
  snprintf(&report_path[0], report_path_len, txt_path_fmt,
           this->out_dir_.c_str(), report_name);
  FILE *fp = fopen(&report_path[0], "w");
  if (fp) {
    fprintf(fp, "Results\n-------\n");
    fclose(fp);
  } else {
    fprintf(stderr, "Warning: unable to open file: %s\n", &report_path[0]);
  }
}

double Results::mean_absolute_error(const pathest::PathData &output) const {
#ifdef DEBUG
  // Invariant: output has the same number of data points as reference.
  if (!this->ref_data_.empty()) {
    assert(this->ref_data_.size() == output.size());
  }
#endif
  if (this->ref_data_.empty()) {
    return 0;
  } else {
    double sum = 0.0;
    double num = 0.0;
    pathest::PathData::const_iterator out_it = output.begin();
    pathest::PathData::const_iterator ref_it = this->ref_data_.begin();
    while (out_it != output.end() && ref_it != this->ref_data_.end()) {
      double x_squared = pow(out_it->x() - ref_it->x(), 2);
      double y_squared = pow(out_it->y() - ref_it->y(), 2);
#ifdef DEBUG
      assert(x_squared + y_squared >= 0);
#endif
      double error = sqrt(x_squared + y_squared);
      sum += error;
      ++out_it;
      ++ref_it;
      ++num;
    }
#ifdef DEBUG
    assert(num != 0);
#endif
    return sum / num;
  }
}

double Results::root_mean_square_error(const pathest::PathData &output) const {
#ifdef DEBUG
  // Invariant: output has the same number of data points as reference.
  if (!this->ref_data_.empty()) {
    assert(this->ref_data_.size() == output.size());
  }
#endif
  if (this->ref_data_.empty()) {
    return 0;
  } else {
    double sum = 0.0;
    double num = 0.0;
    pathest::PathData::const_iterator out_it = output.begin();
    pathest::PathData::const_iterator ref_it = this->ref_data_.begin();
    while (out_it != output.end() && ref_it != this->ref_data_.end()) {
      double dx = out_it->x() - ref_it->x();
      double dy = out_it->y() - ref_it->y();
      double error_squared = dx * dx + dy * dy;
      sum += error_squared;
      ++out_it;
      ++ref_it;
      ++num;
    }
#ifdef DEBUG
    assert(num != 0);
    assert(sum / num >= 0);
#endif
    return sqrt(sum / num);
  }
}

double Results::mean_absolute_scaled_error(const pathest::PathData &output)
  const {
#ifdef DEBUG
  // Invariant: output has the same number of data points as reference.
  if (!this->ref_data_.empty()) {
    assert(this->ref_data_.size() == output.size());
  }
#endif
  if (this->ref_data_.size() < 2) {
    return 0;
  } else {
    double sum = 0.0;
    double num = 0.0;
    pathest::PathData::const_iterator it = this->ref_data_.begin();
    double prev_x = it->x();
    double prev_y = it->y();
    ++it;
    while (it != this->ref_data_.end()) {
      double curr_x = it->x();
      double curr_y = it->y();
      double dx = curr_x - prev_x;
      double dy = curr_y - prev_y;
#ifdef DEBUG
      assert(dx * dx + dy * dy >= 0);
#endif
      sum += sqrt(dx * dx + dy * dy);
      prev_x = curr_x;
      prev_y = curr_y;
      ++it;
      ++num;
    }
#ifdef DEBUG
    assert(num != 0);
    assert(sum / num != 0);
#endif
    double denom = sum / num;

    sum = 0.0;
    num = 0.0;
    pathest::PathData::const_iterator out_it = output.begin();
    pathest::PathData::const_iterator ref_it = this->ref_data_.begin();
    while (out_it != output.end() && ref_it != this->ref_data_.end()) {
      double dx = out_it->x() - ref_it->x();
      double dy = out_it->y() - ref_it->y();
#ifdef DEBUG
      assert(dx * dx + dy * dy >= 0);
#endif
      double error = sqrt(dx * dx + dy * dy);
      sum += error / denom;
      ++out_it;
      ++ref_it;
      ++num;
    }
#ifdef DEBUG
    assert(num != 0);
#endif
    return sum / num;
  }
}
