import sys
import time
import random
import argparse

import frame_printer

from luma.led_matrix.device import max7219
from luma.core.interface.serial import spi, noop
from luma.core.render import canvas

serial = spi(port=0, device=0, gpio=noop())
device = max7219(serial, cascaded=1, block_orientation=-90, rotate=0)
width, height = 0, 0

def readSettingsFile():
    file = open("settings.txt", "r")
    amountMatrixes = int(file.readline())
    global device
    device = max7219(serial, cascaded=amountMatrixes, block_orientation=-90, rotate=0)
    global width
    width = int(file.readline())
    global height
    height = int(file.readline())
    print ("printing in format: {}x{}").format(width, height)

def parseArguments():
    parser = argparse.ArgumentParser()
    parser.add_argument("--delay", help="delay between frames", type=int, default=100)
    parser.add_argument("--start", help="starting point (left, right, top, bottom)", type=str, default="left")
    parser.add_argument("--cycles", help="how often shall the process run", type=int, default=1)
    args = parser.parse_args()
    return args

def SingleLeftRight(delay, cycles):
    delayEach = delay/(width-1)
    for n in range(cycles):
        for i in range(10):
            y = random.randint(0, height-1)
            for x in range (0, width):
                with canvas(device) as draw:
                    point = frame_printer.get_point_for_chained_matrixes(x, y)
                    draw.point(point, fill="white")
                    time.sleep(delayEach)

def SingleRightLeft(delay, cycles):
    delayEach = delay/(width-1)
    for n in range(cycles):
        for i in range(10):
            y = random.randint(0,height-1)
            for x in range (width-1, -1, -1):
                with canvas(device) as draw:
                    #print ("x: {}, y: {}".format(x,y))
                    point = frame_printer.get_point_for_chained_matrixes(x, y)
                    draw.point(point, fill="white")
                    time.sleep(delayEach)

def SingleTopBottom(delay, cycles):
    delayEach = delay/(height-1)
    for n in range(cycles):
        for i in range(10):
            x = random.randint(0, width-1)
            for y in range (0, height):
                with canvas(device) as draw:
                    point = frame_printer.get_point_for_chained_matrixes(x, y)
                    draw.point(point, fill="white")
                    time.sleep(delayEach)

def SingleBottomTop(delay, cycles):
    delayEach = delay/(height-1)
    for n in range(cycles):
        for i in range(10):
            x = random.randint(0, width-1)
            for y in range (height-1, -1, -1):
                with canvas(device) as draw:
                    point = frame_printer.get_point_for_chained_matrixes(x, y)
                    draw.point(point, fill="white")
                    time.sleep(delayEach)




if __name__ == '__main__':
    readSettingsFile()
    args = parseArguments()

    cycles = args.cycles
    delay = float(args.delay)/1000
    print ("delay: {}").format(delay)
    mode = args.start
    print mode
    if mode == "left":
        SingleLeftRight(delay, cycles)
    elif mode == "right":
        SingleRightLeft(delay, cycles)
    elif mode == "top":
        SingleTopBottom(delay, cycles)
    else:
        SingleBottomTop(delay, cycles)


