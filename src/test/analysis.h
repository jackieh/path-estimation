/// @file test/analysis.cc
/// @brief Helper function for interfacing with the path estimation library.
//===----------------------------------------------------------------------===//

#ifndef TEST_ANALYSIS_H_
#define TEST_ANALYSIS_H_

#include "pathest/path.h"
#include "test/results.h"

/// @brief Perform analysis on the input data based on the given config file.
///
/// @param config Configuration file path.
/// @param input Path object to analyze.
/// @param res Results object.
void perform_analysis(const char *config, const pathest::Path &input,
                      const Results &res);

#endif  // TEST_ANALYSIS_H_
