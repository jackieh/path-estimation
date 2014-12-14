/// @file test/parse.cc
/// @brief Helper functions for parsing JSON from a text file.
/// @bug C++ code needs more documentation.
//===----------------------------------------------------------------------===//

#include "test/parse.h"

#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>

#include "nxjson/nxjson.h"
#include "pathest/path_data.h"

bool check_file(const char *filename, struct stat *st) {
#ifdef DEBUG
  // Invariant: given pointers are not null.
  assert(filename);
  assert(st);
#endif
  if (stat(filename, st) < 0) {
    fprintf(stderr, "Can't find file: %s\n", filename);
    return false;
  } else if (!S_ISREG(st->st_mode)) {
    fprintf(stderr, "Not a regular file: %s\n", filename);
    return false;
  } else {
    return true;
  }
}

bool check_stream(const char *filename, const FILE *fp) {
#ifdef DEBUG
  assert(filename);  // Invariant: this pointer is not null.
#endif
  if (!fp) {
    fprintf(stderr, "Unable to open file: %s\n", filename);
    return false;
  } else {
    return true;
  }
}

bool read_file(const char *filename, char *buf, const size_t len,
               FILE *fp) {
#ifdef DEBUG
  // Invariant: given pointers are not null.
  assert(filename);
  assert(buf);
  assert(fp);
#endif
  if (fread(buf, sizeof(char), len, fp) != len) {
    fprintf(stderr, "Can't read file: %s\n", filename);
    fclose(fp);
    return false;
  } else {
    fclose(fp);
    buf[len] = '\0';
    return true;
  }
}

bool check_json(const nx_json *json) {
  if (!json) {
    fprintf(stderr, "Failed to parse JSON format\n");
    return false;
  } else if (json->type != NX_JSON_OBJECT) {
    fprintf(stderr, "Unexpected JSON format: object not found\n");
    nx_json_free(json);
    return false;
  } else {
    return true;
  }
}

nx_json *find_property(const nx_json *json, const nx_json_type type,
                       const char *key) {
#ifdef DEBUG
  // Invariant: given pointers are not null.
  assert(json);
  assert(key);
#endif
  nx_json *prop = NULL;
  for (nx_json *node = json->child; node; node = node->next) {
    if (node->type == type) {
      if (node->key && !strcmp(node->key, key) && !prop) {
        prop = node;
      }
    }
  }
  return prop;
}

bool check_property(const nx_json *json, const nx_json *prop, const char *key) {
#ifdef DEBUG
  // Invariant: these pointers are not null.
  assert(json);
  assert(key);
#endif
  if (!prop) {
    fprintf(stderr, "Unexpected JSON format: \"%s\" property not found\n", key);
    nx_json_free(json);
    return false;
  } else {
    return true;
  }
}

bool check_data(const pathest::PathData &data, bool msg) {
  if (data.empty()) {
    if (msg) fprintf(stderr, "Invalid data: empty data set\n");
    return false;
  } else if (data.min_t() == data.max_t()) {
    if (msg) fprintf(stderr, "Invalid data: identical timestamps\n");
    return false;
  } else if (data.min_x() == data.max_x()) {
    if (msg) fprintf(stderr, "Invalid data: identical x values\n");
    return false;
  } else if (data.min_y() == data.max_y()) {
    if (msg) fprintf(stderr, "Invalid data: identical y values\n");
    return false;
  } else {
    return true;
  }
}
