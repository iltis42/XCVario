#!/usr/bin/python

import argparse
import re
import struct

# Parameters
scale_factor = 100  # e.g., multiply float by 1000 to convert to fixed-point

def proc_tupel(a, b):
    rnda = round(a)
    scale_factor = rnda/a
    corrb = scale_factor * b
    m100b = round(100 * corrb)
    err = m100b - int(m100b)
    formatted = "%3d, %4d, " % (rnda, int(m100b))
    return formatted, err

def process_line(line, line_num):
    pattern = r'\{\s*(\d+),\s*"([^"]+)",\s*(.+?)\s*\}'
    match = re.search(pattern, line, re.VERBOSE)

    if match:
        idx = int(match.group(1))
        name = match.group(2)
        rest = match.group(3).strip().split(',')

        # Convert the remaining values
        numbers = [float(x.strip()) for x in rest if x.strip()]

        # Index, name, load
        m100load = int(100 * numbers[0])
        output = "\t{ %d, \"%s\", %d, " % (idx, name, m100load)
        errout = "\tL: %.4f, " % (100 * numbers[0] - m100load)

        # Example: group as tuples (float, int)
        numbers = numbers[1:]

        # Skip the first value, and the string is ignored
        for i in 0,2,4:
            tp, err = proc_tupel(numbers[i], numbers[i+1])
            output += tp
            errout += "P: %.4f " % err

        ballast =  int(numbers[6]/2) # half of the max. ballast.
        wingarea = int(round(100 * numbers[7]))
        wingerr = 100 * numbers[7] - wingarea
        flaps = numbers[8]

        output += "%3d, %5d, %1d }," % (ballast, wingarea, flaps)
        errout += "\tW %.4f" % wingerr

        #print()
        #print(line.rstrip())
        #print (output)
        #print(errout)
        return output


parser = argparse.ArgumentParser()
parser.add_argument('--input', required=True)
parser.add_argument('--output', required=True)
args = parser.parse_args()

output_lines = []

print("Read polars from:", args.input)
with open(args.input, 'r') as f:
    for line_num, line in enumerate(f, 1):
        output_lines.append(process_line(line, line_num))

print("Write packed polares to:", args.output)
with open(args.output, 'w') as outfile:
    for line in output_lines:
        outfile.write(line + "\n")
