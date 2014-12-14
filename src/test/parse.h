/// @file test/parse.cc
/// @brief Helper functions for parsing JSON from a text file.
/// @bug Header file needs documentation.
//===----------------------------------------------------------------------===//

#ifndef TEST_PARSE_H_
#define TEST_PARSE_H_

#include <stdio.h>
#include <sys/stat.h>

#include "nxjson/nxjson.h"
#include "pathest/path_data.h"

bool check_stream(const char *filename, const FILE *fp);
bool check_file(const char *, struct stat *);
bool read_file(const char *, char *, const size_t, FILE *);
bool check_json(const nx_json *);
nx_json *find_property(const nx_json *, const nx_json_type, const char *);
bool check_property(const nx_json *, const nx_json *, const char *);
bool check_data(const pathest::PathData &, bool);

#endif  // TEST_PARSE_H_
