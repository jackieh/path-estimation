/// @file test/main.cc
/// @brief Main program for path estimation test.
//===----------------------------------------------------------------------===//

#include <stdio.h>
#include <sys/stat.h>

#include "json/json.h"
#include "pathest/path_data.h"
#include "test/analysis.h"
#include "test/parse.h"
#include "test/results.h"

// Fill an existing data object with the contents of a given file.
bool parse_data(const char *, pathest::PathData *);

int main(int argc, const char *argv[]) {
  if (argc < 4) {
    fprintf(stderr, "Usage: %s <analysis config> <input file>"
            " <output directory> <optional reference file>\n", argv[0]);
    return -1;
  }

  // Gather input data.
  pathest::PathData report_data;
  if (!parse_data(argv[2], &report_data)) {
    fprintf(stderr, "Failed to read input data from %s\n", argv[2]);
    return -1;
  }

  // Check the output directory.
  struct stat st;
  if (stat(argv[3], &st) < 0 || !S_ISDIR(st.st_mode)) {
    fprintf(stderr, "No such directory: %s\n", argv[3]);
    return -1;
  }
  Results res(argv[3], report_data);

  // Check for optional reference file. Continue if there is an error.
  if (argc > 4) {
    pathest::PathData reference_data;
    if (parse_data(argv[4], &reference_data)) {
      res.add_reference(reference_data);
    } else {
      fprintf(stderr, "Warning: unable to read reference data\n");
    }
  }

  res.write("input", "Input data", report_data);  // Write the input data.
  perform_analysis(argv[1], report_data, res);  // Compute and write results.
  return 0;
}

bool parse_data(const char *filename, pathest::PathData *data) {
  Json::Value root;
  if (!get_json(filename, &root)) return false;

  // Get properties.
  Json::Value target = root["target"];
  Json::Value reports = root["reports"];
  if (!target.isString()) {
    fprintf(stderr, "\"target\" string not found in JSON input\n");
    return false;
  } else if (target.asString().compare("train")) {
    fprintf(stderr, "Unexpected \"target\" value in JSON input\n");
    return false;
  } else if (!reports.isArray()) {
    fprintf(stderr, "\"reports\" list not found in JSON input\n");
    return false;
  }

  // Add data.
  for (unsigned i = 0; i < reports.size(); ++i) {
    if (reports[i].isObject()) {
      Json::Value x = reports[i]["x"];
      Json::Value y = reports[i]["y"];
      Json::Value t = reports[i]["timestamp"];
      if (x.isDouble() && y.isDouble() && t.isDouble()) {
        data->add_point(x.asDouble(), y.asDouble(), t.asDouble());
      }
    }
  }

  // Verify data.
  if (data->empty()) {
    fprintf(stderr, "Invalid data: empty data set\n");
    return false;
  } else if (data->min_t() == data->max_t()) {
    fprintf(stderr, "Invalid data: identical timestamps\n");
    return false;
  } else if (data->min_x() == data->max_x()) {
    fprintf(stderr, "Invalid data: identical x values\n");
    return false;
  } else if (data->min_y() == data->max_y()) {
    fprintf(stderr, "Invalid data: identical y values\n");
    return false;
  } else {
    return true;
  }
}
