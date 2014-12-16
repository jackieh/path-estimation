/// @file test/parse.h
/// @brief Helper functions for parsing JSON from a text file.
/// @bug Needs documentation.
//===----------------------------------------------------------------------===//

#ifndef TEST_PARSE_H_
#define TEST_PARSE_H_

#include "json/json.h"

bool get_json(const char *filename, Json::Value *json);

#endif  // TEST_PARSE_H_
