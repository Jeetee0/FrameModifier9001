from luma.led_matrix.device import max7219
from luma.core.interface.serial import spi, noop
from luma.core.render import canvas
from luma.core.virtual import viewport
from luma.core.legacy import text, show_message
from luma.core.legacy.font import proportional, CP437_FONT, TINY_FONT, SINCLAIR_FONT, LCD_FONT

import re
import time
import argparse
import random

serial = spi(port=0, device=0, gpio=noop())
device = max7219(serial, cascaded=16, block_orientation=-90, rotate=0)
#show_message(device, "123456789", fill="white", font=proportional(CP437_FONT))
#   x-->
#  y
#  |
#  |
#
#

for x in range(0, 32):
    for y in range (0, 8):
        with canvas(device) as draw:
            #print ("x: {}, y: {}".format(x,y))
            draw.point((x, y), fill="white")
            time.sleep(0.001)

for y in range(0, 8):
    for x in range (0, 32):
        with canvas(device) as draw:
            #print ("x: {}, y: {}".format(x,y))
            draw.point((x, y), fill="white")
            time.sleep(0.001)


while (True):
    y = random.randint(0,8)
    for x in range (0, 32):
        with canvas(device) as draw:
            #print ("x: {}, y: {}".format(x,y))
            draw.point((x, y), fill="white")
            time.sleep(0.001)