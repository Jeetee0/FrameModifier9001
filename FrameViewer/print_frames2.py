import sys
import os
import time
import frames_lib
import argparse

frameList = []

def read_frame_list(filename, delaytime):
    file = open(filename, 'r')
    # first line is delaytime
    global delay
    if (delaytime == 0):
        delay = float(file.readline())/1000
    else:
        delay = float(delaytime)/1000
            
    # every other line is array with frame to print
    for line in file.readlines()[1:]: #skip first line because it is delaytime
        lineWithoutBreak = line.split('\n')
        array = lineWithoutBreak[0].split(' ')
        
        # fill symbol list with 4 matrixes
        symbol = []
        for matrix in array:
            matrixList = matrix.split(',')
            symbol.append(matrixList)
        frameList.append(symbol)
    file.close()

read_frame_list('frames.txt', 1000)
x = 0
for frame in frameList:
    frames_lib.print_8x8(frame[0], x)
    time.sleep(delay)
    if x>120:
        x=0
    else:
        x= x+8
    print (x)