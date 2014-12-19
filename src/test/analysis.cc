/// @file test/analysis.cc
/// @brief Helper function for interfacing with the path estimation library.
///
/// The perform_analysis function currently tests three types of path
/// smoothing: the simple moving average, exponential smoothing, and a Kalman
/// filter. The config file it reads from specifies parameters for each type of
/// test.
///
/// The simple moving average takes the sample size (the number of data points
/// factored into each average) and the number of iterations to run over the
/// entire data set. An iteration number of 1 would simulate real-time
/// data analysis.
///
/// Exponential smoothing takes the smoothing factor (a float between 0 and 1)
/// and the number of iterations to run over the entire data set. An iteration
/// number of 1 would simulate real-time data analysis.
///
/// Currently the Kalman filter has its initial state hardcoded, but this may
/// change in the future.
///
//===----------------------------------------------------------------------===//

#include "test/analysis.h"

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <utility>
#include <vector>

#include "json/json.h"
#include "pathest/path.h"
#include "test/parse.h"
#include "test/results.h"

// There are up to 10 digits in a decimal representation of a uint32_t value.
#define UINT32_MAX_DIGITS 10

// Types for analysis parameters.
typedef std::pair<int, int> sma_param_t;
typedef std::pair<int, double> es_param_t;
typedef std::vector<sma_param_t> sma_params_t;
typedef std::vector<es_param_t> es_params_t;

typedef struct AnalysisParams {
  AnalysisParams() :
    sma_params(std::vector<sma_param_t>()),
    es_params(std::vector<es_param_t>()), use_kf(false) {}
  ~AnalysisParams() {}

  sma_params_t sma_params;
  es_params_t es_params;
  bool use_kf;
} analysis_params_t;

// Templates for plot names and titles.
const char *sma_name = "out-sma-%d";
const size_t sma_name_len = strlen(sma_name) + UINT32_MAX_DIGITS + 1 - 2;
const char *sma_title =
  "Simple moving average with %d iteration(s) and %d sample(s)";
const size_t sma_title_len =
  strlen(sma_title) + (2 * UINT32_MAX_DIGITS) + 1 - 4;

const char *es_name = "out-es-%d";
const size_t es_name_len = strlen(es_name) + UINT32_MAX_DIGITS + 1 - 2;
const char *es_title =
  "Exponential smoothing with %d iteration(s) and smoothing factor %.4f";
const size_t es_title_len = strlen(es_title) + UINT32_MAX_DIGITS + 6 + 1 - 6;

const char *kf_name = "out-kf";
const char *kf_title = "Kalman filter";

// Fill an existing params struct with the contents of a given file.
bool parse_params(const char *, analysis_params_t *);

void perform_analysis(const char *config, const pathest::Path &input,
                      const Results &res) {
  analysis_params_t params;
  if (parse_params(config, &params)) {
    uint32_t count;
    pathest::Path est_data;

    // Simple moving average analysis.
    count = 0;
    for (sma_params_t::const_iterator it = params.sma_params.begin();
         it != params.sma_params.end(); ++it) {
      est_data = input;
      int iterations = it->first;
      int samples = it->second;
      for (int i = 0; i < iterations; ++i) {
        est_data = est_data.simple_moving_average(samples);
      }
      std::vector<char> name(sma_name_len);
      std::vector<char> title(sma_title_len);
      snprintf(&name[0], sma_name_len, sma_name, count);
      snprintf(&title[0], sma_title_len, sma_title, iterations, samples);
      res.write(&name[0], &title[0], est_data);
      ++count;
    }

    // Exponential smoothing analysis.
    count = 0;
    for (es_params_t::const_iterator it = params.es_params.begin();
         it != params.es_params.end(); ++it) {
      est_data = input;
      int iterations = it->first;
      double smoothing = it->second;
      for (int i = 0; i < iterations; ++i) {
        est_data = est_data.exponential_smoothing(smoothing);
      }
      std::vector<char> name(es_name_len);
      std::vector<char> title(es_title_len);
      snprintf(&name[0], es_name_len, es_name, count);
      snprintf(&title[0], es_title_len, es_title, iterations, smoothing);
      res.write(&name[0], &title[0], est_data);
      ++count;
    }

    // Kalman filter analysis.
    if (params.use_kf) {
      est_data = input.kalman_filter();
      res.write(kf_name, kf_title, est_data);
    }
  }
}

bool parse_params(const char *filename, analysis_params_t *params) {
  Json::Value root;
  if (!get_json(filename, &root)) return false;

  Json::Value sma = root["sma"];
  Json::Value es = root["es"];
  Json::Value kf = root.get("kf", false);

  // Kalman filter parameters.
  if (kf.isBool()) params->use_kf = kf.asBool();

  // Simple moving average parameters.
  if (sma.isArray()) {
    for (unsigned i = 0; i < sma.size(); ++i) {
      if (sma[i].isObject()) {
        Json::Value iterations = sma[i]["iterations"];
        Json::Value samples = sma[i]["samples"];
        if (iterations.isInt() && samples.isInt()) {
          int i = iterations.asInt();
          int s = iterations.asInt();
          if (i > 0 && s > 0) params->sma_params.push_back(sma_param_t(i, s));
        }
      }
    }
  }

  // Exponential smoothing parameters.
  if (es.isArray()) {
    for (unsigned i = 0; i < es.size(); ++i) {
      Json::Value iterations = es[i]["iterations"];
      Json::Value smoothing = es[i]["smoothing"];
      if (iterations.isInt() && smoothing.isDouble()) {
        int i = iterations.asInt();
        double s = smoothing.asDouble();
        if (i > 0 && s >= 0 && s <= 1) {
          params->es_params.push_back(es_param_t(i, s));
        }
      }
    }
  }

  return true;
}
