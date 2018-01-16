import sys
import os
import time

# in general my generated files contain bit values representing a full column for a frame
# first line defines dimensions: size of width and height
# second line is delaytime

def read_frame_list(filename):
    file = open(filename, 'r')

    frameList = []
    # every other line is array with frame to print
    # skip first two lines
    for line in file.readlines()[2:]:
        lineWithoutBreak = line.split('\n')
        array = lineWithoutBreak[0].split(',')
        #print array
        frameList.append(array)

    file.close()
    return frameList

def get_delay_from_file(filename):
    file = open(filename, 'r')
    for i, line in enumerate(file):
        if i == 1:
            return float(line.split("\n")[0])/1000

def get_size_of_frame(filename):
    file = open(filename, 'r')
    return file.readline().split("\n")[0]