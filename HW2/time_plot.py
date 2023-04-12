#!/usr/bin/env python3

# Import
import getopt
import matplotlib.pyplot as plt
import numpy
import re
import sys

def main(argv):
    #Parse arguments
    input_file = ''
    output_file = ''
    opts, args = getopt.getopt(argv,"hi:o:",["ifile=","ofile="])
    for opt, arg in opts:
        if opt == '-h':
            print ('solution_plot.py -i <inputfile> -o <outputfile>')
            sys.exit()
        elif opt in ("-i", "--ifile"):
            input_file = arg
        elif opt in ("-o", "--ofile"):
            output_file = arg

    x = numpy.arange(1, 9, 1, dtype=int)
    y_init = numpy.empty(3, dtype=object)
    y = numpy.empty(3, dtype=object)
    y_tmp = numpy.zeros(8, dtype=float)
    count = 0
    iterate = 0
    with open(input_file) as time_data:
      for line in time_data.readlines():
        val = re.findall("\d+\.\d+", line)
        if val:
          value = float(val[0])
          y_tmp[count] = value
          count += 1
        if count == 8:
          count = 0
          y_init[iterate] = y_tmp
          iterate += 1
          y_tmp = numpy.zeros(8, dtype=float)

    for i in range(3):
      y[i] = y_init[i][0] / y_init[i]

    fig, ax = plt.subplots()

    ax.plot(x,y[0], 'r-o', label='Nt')
    ax.plot(x,y[1], 'b-o', label='2*Nt')
    ax.plot(x,y[2], 'g-o', label='3*Nt')

    ax.set_title('Speed-Up vs. Number of Threads')
    ax.set_xlabel('Number of Threads')
    ax.set_ylabel('Speed-Up')

    leg = ax.legend(loc=2)
    plt.savefig("figures/speed_up.png")

if(__name__ == '__main__'):
    try:
        sys.exit(main(sys.argv[1:]))
    except IOError:
        exit(1)
