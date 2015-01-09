/// @file test/parse.h
/// @brief Helper functions for parsing JSON from a text file.
//===----------------------------------------------------------------------===//

#ifndef TEST_PARSE_H_
#define TEST_PARSE_H_

#include "json/json.h"

/// @brief Get a Json object initialized from the contents of a given file.
///
/// @param filename Path to Json text file.
/// @param json New Json object.
/// @returns true if successful, false otherwise.
bool get_json(const char *filename, Json::Value *json);

#endif  // TEST_PARSE_H_
