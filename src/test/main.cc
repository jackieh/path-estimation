/// @file test/main.cc
/// @brief Main program for path estimation test.
//===----------------------------------------------------------------------===//

#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <memory>

#include "nxjson/nxjson.h"
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
  // Check and read a file.
  struct stat st;
  if (!check_file(filename, &st)) return false;
  FILE *fp = fopen(filename, "rb");
  if (!check_stream(filename, fp)) return false;
  std::vector<char> buf(st.st_size + 1);
  buf[st.st_size] = '\0';
  if (!read_file(filename, &buf[0], (size_t)st.st_size, fp)) return false;
  const nx_json *json = nx_json_parse_utf8(&buf[0]);
  if (!check_json(json)) return false;

  // Find string property "target" and array property "reports".
  nx_json *target = find_property(json, NX_JSON_STRING, "target");
  nx_json *reports = find_property(json, NX_JSON_ARRAY, "reports");
  if (!check_property(json, target, "target")) return false;
  if (!check_property(json, reports, "reports")) return false;

  // Check the "target" property.
  if (!target->text_value || strcmp(target->text_value, "train")) {
    fprintf(stderr, "Unexpected JSON format: wrong \"target\" value\n");
    nx_json_free(json);
    return false;
  }

  // Check the "reports" property, and add elements.
  for (nx_json *report = reports->child; report; report = report->next) {
    nx_json *x = find_property(report, NX_JSON_DOUBLE, "x");
    nx_json *y = find_property(report, NX_JSON_DOUBLE, "y");
    nx_json *timestamp = find_property(report, NX_JSON_DOUBLE, "timestamp");

    // Skip over a report if it doesn't have all three data points.
    if (!x) {
      fprintf(stderr, "Warning: \"x\" property not found\n");
    } else if (!y) {
      fprintf(stderr, "Warning: \"y\" property not found\n");
    } else if (!timestamp) {
      fprintf(stderr, "Warning: \"timestamp\" property not found\n");
    } else {
      data->add_point(x->dbl_value, y->dbl_value, timestamp->dbl_value);
    }
  }

  nx_json_free(json);
  return check_data(*data, true);
}
