#!/usr/bin/env python3

# Import
import getopt
import matplotlib.pyplot as plt
import numpy
import sys

from mpl_toolkits.mplot3d.axes3d import Axes3D

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

    X = numpy.outer(numpy.linspace(0, 1, 6401), numpy.ones(6401))
    Y = X.copy().T
    Z = numpy.genfromtxt(input_file, dtype=float)
    
    fig = plt.figure(figsize =(14,9))
    ax = plt.axes(projection = '3d')

    cmap_init = plt.get_cmap('hot')
    
    #surf = ax.plot_surface(X, Y, Z, cmap = cmap_init, edgecolor='none')
    surf = ax.plot_wireframe(X, Y, Z)

    fig.colorbar(surf, ax = ax, shrink = 0.5, aspect = 5)

    plt.show()

if(__name__ == '__main__'):
    try:
        sys.exit(main(sys.argv[1:]))
    except IOError:
        exit(1)
