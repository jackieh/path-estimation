/// @file test/analysis.cc
/// @brief Helper function for interfacing with the path estimation library.
/// @bug May need more documentation.
//===----------------------------------------------------------------------===//

#ifndef TEST_ANALYSIS_H_
#define TEST_ANALYSIS_H_

#include "pathest/path.h"
#include "test/results.h"

/// @brief Perform analysis on the input data based on the given config file.
void perform_analysis(const char *config, const pathest::Path &input,
                      const Results &res);

#endif  // TEST_ANALYSIS_H_
