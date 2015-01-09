#!/usr/bin/env python
# pylint: disable=W0141

"""Generates data points for a path following a mathematical function of x.

This script is mostly intended as a crude wrapper around the Python shell. It
takes optional arguments describing a mathematical expression in terms of
variable x, the number of data points to generate, and the starting value of x.
The generated data points follow the function for increasing values of x,
maintaining an approximately constant velocity.

This script is not intended for API use, and is currently too unsafe for such a
purpose. It doesn't check for user input validity, and uses the eval() function
to evaluate the mathematical expression.

Example usage:
  ./generate.py -c
  ./generate.py --fun="50 * math.sin(x / 20)"
  ./generate.py -n 200 -m 20 -f "math.sqrt(x) + (((x - 100) ** 3) / 10000)"
"""

import decimal
import json
import math
import numpy
import optparse
import os
import random

_MAX_NUM_TESTS = 100

_REPORT_TEMPLATE = {"target": "train"}

_REPORT_KEY = "reports"


def filename(i):
    """Return a pair of file paths for a given id number."""
    rand_name = os.path.join(os.getcwd(), "input-%d.txt" % i)
    ref_name = os.path.join(os.getcwd(), "input-%d.ref" % i)
    return rand_name, ref_name


def clear_data():
    """Clear out all generated reports and exit."""
    for i in range(_MAX_NUM_TESTS):
        rand, ref = filename(i)
        if os.path.exists(rand):
            os.remove(rand)
        if os.path.exists(ref):
            os.remove(ref)


def generate_id():
    """Return an unused id number."""
    for i in range(_MAX_NUM_TESTS):
        test, ref = filename(i)
        if not os.path.exists(test) and not os.path.exists(ref):
            return i
    return None


class DataGen:
    """Class for generating input data and reference for a test."""

    stdev_distance = 14.4841  # KM
    speed = 200.0 / 60.0  # KPM
    precision = 10

    default_start = 0
    default_expr = "x"
    default_num = 100

    def __init__(self, start, expr, num):
        """Set test parameters to given arguments if valid."""
        self.data = []
        if start is None:
            self.start = DataGen.default_start
            print ("Warning: Using default minimum x value (%d)"
                   % (DataGen.default_start))
        else:
            self.start = start
        if expr is None:
            self.expr = DataGen.default_expr
            print ("Warning: Using default function (%s)"
                   % (DataGen.default_expr))
        else:
            self.expr = expr
        if num is None:
            self.num = DataGen.default_num
            print ("Warning: Using default number of data points (%d)"
                   % (DataGen.default_num))
        else:
            self.num = num

    @staticmethod
    def __randomize_coord((ref_x, ref_y)):
        """Return a randomized coordinate pair given a reference."""
        radius = numpy.random.normal(scale=DataGen.stdev_distance)
        angle = random.uniform(0, 2 * math.pi)
        rand_x = ref_x + radius * math.cos(angle)
        rand_y = ref_y + radius * math.sin(angle)
        return rand_x, rand_y

    @staticmethod
    def __round(num):
        """Round a float down to the class precision."""
        return float(round(decimal.Decimal(num), DataGen.precision))

    @staticmethod
    def __make_json((time, (x_coord, y_coord))):
        """Return a JSON object given a location report."""
        return {'timestamp': time, 'x': x_coord, 'y': y_coord}

    def __randomize_data(self):
        """Return a randomized data set."""
        times = map(lambda (t, _): t, self.data)
        coords = map(lambda (_, c): c, self.data)
        return zip(times, map(DataGen.__randomize_coord, coords))

    def __evaluate(self, var_x):
        """Return f(x) with hacks."""
        # pylint: disable=W0612,C0103
        x = var_x
        # pylint: enable=W0612,C0103
        return eval(self.expr)

    def __get_coords(self, prev_x, prev_y, delta_t):
        """Approximate the next coordinate pair."""
        orig_x = DataGen.__round(prev_x)
        orig_y = DataGen.__round(prev_y)
        dist = DataGen.__round(DataGen.speed * delta_t)
        min_x = DataGen.__round(prev_x)
        max_x = DataGen.__round(prev_x + dist)
        while min_x < max_x:
            med_x = DataGen.__round((min_x + max_x) / 2)
            med_y = DataGen.__round(self.__evaluate(med_x))
            delta_x = med_x - orig_x
            delta_y = med_y - orig_y
            radius = DataGen.__round(math.sqrt(delta_x ** 2 + delta_y ** 2))
            if radius < dist:
                min_x = med_x + (1.0 / 10 ** DataGen.precision)
            elif radius > dist:
                max_x = med_x - (1.0 / 10 ** DataGen.precision)
            else:
                return med_x, med_y
        return min_x, DataGen.__round(self.__evaluate(min_x))

    def generate_data(self):
        """Generate data points."""
        time = 0
        ref_x = self.start
        ref_y = self.__evaluate(ref_x)
        for _ in range(self.num):
            delta_t = random.uniform(0, 1)
            (ref_x, ref_y) = self.__get_coords(ref_x, ref_y, delta_t)
            time += delta_t
            self.data.append((time, (ref_x, ref_y)))

    def write_ref(self, report_id):
        """Write reference data to files."""
        reports = _REPORT_TEMPLATE.copy()
        reports[_REPORT_KEY] = map(DataGen.__make_json, self.data)
        _, ref_filename = filename(report_id)
        ref_file = open(ref_filename, 'w')
        ref_file.write(json.dumps(reports, indent=2))
        ref_file.close()

    def write_rand(self, report_id):
        """Write randomized data to files."""
        reports = _REPORT_TEMPLATE.copy()
        reports[_REPORT_KEY] = map(DataGen.__make_json, self.__randomize_data())
        rand_filename, _ = filename(report_id)
        rand_file = open(rand_filename, 'w')
        rand_file.write(json.dumps(reports, indent=2))
        rand_file.close()


def main():
    """Process command line options."""
    parser = optparse.OptionParser()
    parser.add_option("-c", "--clear", action="store_true", dest="clear",
                      help="clear out all generated reports")
    parser.add_option("-n", "--num", action="store", type="int", dest="num",
                      help="number of data points to generate")
    parser.add_option("-m", "--min", action="store", type="float", dest="min",
                      help="minimum of polynomial range")
    parser.add_option("-f", "--fun", action="store", type="string", dest="fun",
                      help=("Python expression (function of x)"))
    (options, _) = parser.parse_args()
    if options.clear:
        clear_data()
    else:
        report_id = generate_id()
        if report_id is None:
            print "Too many tests exist already"
        else:
            gen = DataGen(options.min, options.fun, options.num)
            gen.generate_data()
            gen.write_ref(report_id)
            gen.write_rand(report_id)


if __name__ == "__main__":
    main()
