import time
import random
import argparse

from luma.led_matrix.device import max7219
from luma.core.interface.serial import spi, noop
from luma.core.render import canvas

serial = spi(port=0, device=0, gpio=noop())
device = max7219(serial, cascaded=4, block_orientation=-90, rotate=0)
width, height = 0, 0

def readSettingsFile():
    file = open("../settings.txt", "r")
    global width
    width = int(file.readline())
    global height
    height = int(file.readline())
    print ("printing in format: {}x{}").format(width, height)

def printFull():
    while(True):
        # switch between full printed screen and empty screen
        with canvas(device) as draw:
            for x in range(0, width):
                for y in range (0, height):
                    draw.point((x, y), fill="white")
            time.sleep(delay)
        with canvas(device) as draw:
            time.sleep(delay)


if __name__ == '__main__':

    readSettingsFile()
    parser = argparse.ArgumentParser()
    parser.add_argument("--delay", help="delay between frames", type=int, default=420)
    args = parser.parse_args()
    delay = float(args.delay-30)/1000
    printFull()