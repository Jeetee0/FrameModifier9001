import sys
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

def print_frame_list(cycles):
    endless = None
    roundsLeft = cycles
    if (cycles == 0):
        endless = True
        cycles = sys.maxint
    round = 1
    
    print ("delay between frames: {} ms".format(delay*1000))

    while (endless == True or roundsLeft >= 1):
        print('Round: {}/{}'.format(round, cycles))
        for frame in frameList:
            frames_lib.print_16x16(frame)
            time.sleep(delay)
        
        roundsLeft -= 1
        round += 1
        
        
if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='2x2MatrixProject arguments',
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    
    parser.add_argument('--cycles', type=int, default=1, help='how often shall the program run? 0 for endless')
    parser.add_argument('--delay', type=int, default=0, help='change delaytime (in ms)')
    parser.add_argument('--filename', type=str, default='frames.txt', help='which .txt file should be executed - standard: frames.txt')
    parser.add_argument('--msg', type=str, default='', help='message that gets displayed at the beginning')

    args = parser.parse_args()    
    
    try:
        if (args.msg != ''):
            frames_lib.printText(args.msg)
        read_frame_list(args.filename, args.delay)
        print_frame_list(args.cycles)
    except KeyboardInterrupt:
        pass    