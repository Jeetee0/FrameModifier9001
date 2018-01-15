import time
import random

from luma.led_matrix.device import max7219
from luma.core.interface.serial import spi, noop
from luma.core.render import canvas

serial = spi(port=0, device=0, gpio=noop())
device = max7219(serial, cascaded=4, block_orientation=-90, rotate=0)

while (True):
    y = random.randint(0,32)
    for x in range (0, 8):
        with canvas(device) as draw:
            #print ("x: {}, y: {}".format(x,y))
            draw.point((x, y), fill="white")
            time.sleep(0.001)