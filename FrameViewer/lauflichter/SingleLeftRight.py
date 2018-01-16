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

def SingleLeftRight(delay):
    delayEach = delay/(width-1)
    while (True):
        y = random.randint(0, height-1)
        for x in range (0, width):
            with canvas(device) as draw:
                #print ("x: {}, y: {}".format(x,y))
                draw.point((x, y), fill="white")
                time.sleep(delayEach)

if __name__ == '__main__':
    readSettingsFile()
    parser = argparse.ArgumentParser()
    parser.add_argument("--delay", help="delay between frames", type=int, default=200)
    args = parser.parse_args()
    delay = float(args.delay)/1000
    print ("delay: {}").format(delay)
    SingleLeftRight(delay)