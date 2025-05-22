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

        ballast =  int(numbers[6])
        wingarea = int(round(100 * numbers[7]))
        wingerr = 100 * numbers[7] - wingarea

        output += "%3d, %5d }," % (ballast, wingarea)
        errout += "\tW %.4f" % wingerr

        #print()
        #print(line.rstrip())
        #print (output)
        print(errout)
        return output


output_lines = []

with open("Polars.txt", "r") as f:
    for line_num, line in enumerate(f, 1):
        output_lines.append(process_line(line, line_num))

with open("Polar_pack.txt", "w") as outfile:
    for line in output_lines:
        outfile.write(line + "\n")