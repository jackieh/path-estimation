/// @file test/parse.cc
/// @brief Helper functions for parsing JSON from a text file.
//===----------------------------------------------------------------------===//

#include "test/parse.h"

#include <stdio.h>
#include <sys/stat.h>
#include <string>
#include <vector>

#include "json/json.h"

bool get_json(const char *filename, Json::Value *json) {
  if (!filename || !json) return false;

  struct stat st;
  if (stat(filename, &st) < 0) {
    fprintf(stderr, "Can't find file: %s\n", filename);
    return false;
  } else if (!S_ISREG(st.st_mode)) {
    fprintf(stderr, "Not a regular file: %s\n", filename);
    return false;
  }

  FILE *fp = fopen(filename, "rb");
  if (!fp) {
    fprintf(stderr, "Unable to open file: %s\n", filename);
    return false;
  }

  std::vector<char> buf(st.st_size + 1);
  size_t len = (size_t) st.st_size;
  if (fread(&buf[0], sizeof(char), len, fp) != len) {
    fprintf(stderr, "Can't read file: %s\n", filename);
    fclose(fp);
    return false;
  }
  fclose(fp);
  buf[st.st_size] = '\0';

  Json::Reader reader;
  return reader.parse(std::string(buf.begin(), buf.end()), *json);
}
