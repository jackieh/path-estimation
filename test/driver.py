#!/usr/bin/env python
# pylint: disable=W0141

"""Runs path estimation tests.

Run this after the testing program is built. Expects a specific directory tree
structure in the data directory. Writes results to the out directory if able.

Example usage:
  ./driver.py --all
  ./driver.py --fail --edge
"""

import optparse
import os
import shutil
import subprocess

_BIN = "./estimate"
_CONFIG = "./test/config.json"

_TEST_EXT = ".txt"
_REF_EXT = ".ref"

_OUT_DIR = os.path.join(os.path.dirname(os.path.realpath(__file__)), "out")
_DATA_DIR = os.path.join(os.path.dirname(os.path.realpath(__file__)), "data")

_GEN_DIR = "generated"
_FAIL_DIR = "fail"
_EDGE_DIR = "edge"
_DEMO_DIR = "given"


def green_text(text):
    """Wrap a string in characters that will display it as green when printed.
    """
    return "\033[0;32m%s\033[00m" % text

def red_text(text):
    """Wrap a string in characters that will display it as red when printed."""
    return "\033[0;31m%s\033[00m" % text


def get_test_files(subdir_name):
    """Get list of tests for a given subdirectory."""
    return os.listdir(os.path.join(_DATA_DIR, subdir_name))


def get_test_name(file_name):
    """Return a test name if valid extension, otherwise return blank string."""
    name, ext = os.path.splitext(file_name)
    return name if ext == _TEST_EXT else ""


def get_test_paths(test_name, subdir_name):
    """Return a pair of paths."""
    test_subdir = os.path.join(_DATA_DIR, subdir_name)
    test_path = os.path.join(test_subdir, "%s%s" % (test_name, _TEST_EXT))
    ref_path = os.path.join(test_subdir, "%s%s" % (test_name, _REF_EXT))
    out_path = os.path.join(os.path.join(_OUT_DIR, subdir_name), test_name)
    ref_path = ref_path if os.path.exists(ref_path) else ""
    return (test_path, out_path, ref_path)


def get_test_names(subdir_name):
    """Return a list of test names given a subdirectory."""
    return map(lambda test_name: get_test_paths(test_name, subdir_name),
               filter(bool, map(get_test_name, get_test_files(subdir_name))))


def run_gen():
    """Run tests on generated data."""
    for test, out, ref in get_test_names(_GEN_DIR):
        if os.path.exists(out):
            shutil.rmtree(out)
        os.makedirs(out)
        child = subprocess.Popen([_BIN, _CONFIG, test, out, ref],
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.PIPE)
        status = child.wait()
        if status == 0:
            print "%s: %s" % (green_text("Succeeded as expected"), test)
            print "\tOutput found in %s" % out
        else:
            print "%s: %s" % (red_text("Unexpectedly failed"), test)


def run_fail():
    """Run tests that are expected to fail."""
    for test, out, _ in get_test_names(_FAIL_DIR):
        if os.path.exists(out):
            shutil.rmtree(out)
        os.makedirs(out)
        child = subprocess.Popen([_BIN, _CONFIG, test, out],
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.PIPE)
        status = child.wait()
        if status == 0:
            print "%s: %s" % (red_text("Unexpectedly succeeded"), test)
        else:
            print "%s: %s" % (green_text("Failed as expected"), test)


def run_edge():
    """Run tests on working edge cases."""
    for test, out, _ in get_test_names(_EDGE_DIR):
        if os.path.exists(out):
            shutil.rmtree(out)
        os.makedirs(out)
        child = subprocess.Popen([_BIN, _CONFIG, test, out],
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.PIPE)
        status = child.wait()
        if status == 0:
            print "%s: %s" % (green_text("Succeeded as expected"), test)
            print "\tOutput found in %s" % out
        else:
            print "%s: %s" % (red_text("Unexpectedly failed"), test)


def run_demo():
    """Run tests on given data."""
    for test, out, _ in get_test_names(_DEMO_DIR):
        if os.path.exists(out):
            shutil.rmtree(out)
        os.makedirs(out)
        child = subprocess.Popen([_BIN, _CONFIG, test, out],
                                 stdout=subprocess.PIPE,
                                 stderr=subprocess.PIPE)
        status = child.wait()
        if status == 0:
            print "%s: %s" % (green_text("Succeeded as expected"), test)
            print "\tOutput found in %s" % out
        else:
            print "%s: %s" % (red_text("Unexpectedly failed"), test)


def main():
    """Process command line options."""
    if not os.path.exists(_BIN):
        print "I can't find the binary. Did you run 'make test'?"
        return
    if not os.path.exists(_CONFIG):
        print "I can't find the config file. It should be included in the repo."
        return
    parser = optparse.OptionParser()
    parser.add_option("-g", "--gen", action="store_true", dest="gen",
                      help="Run test on generated data")
    parser.add_option("-f", "--fail", action="store_true", dest="fail",
                      help="Run tests that are expected to fail.")
    parser.add_option("-e", "--edge", action="store_true", dest="edge",
                      help="Run tests on working edge cases.")
    parser.add_option("-d", "--demo", action="store_true", dest="demo",
                      help="Run tests on given data")
    parser.add_option("-a", "--all", action="store_true", dest="all",
                      help="Run all tests.")
    (options, _) = parser.parse_args()
    if options.all or options.gen:
        run_gen()
    if options.all or options.fail:
        run_fail()
    if options.all or options.edge:
        run_edge()
    if options.all or options.demo:
        run_demo()
    if (not options.gen and not options.fail and not options.edge
        and not options.demo and not options.all):
        print "Nothing to do."


if __name__ == "__main__":
    main()
