import re
import time
import argparse

from luma.led_matrix.device import max7219
from luma.core.interface.serial import spi, noop
from luma.core.render import canvas
from luma.core.virtual import viewport
from luma.core.legacy import text, show_message
from luma.core.legacy.font import proportional, CP437_FONT, TINY_FONT, SINCLAIR_FONT, LCD_FONT

# create matrix device
serial = spi(port=0, device=0, gpio=noop())
device = max7219(serial, cascaded=4, block_orientation=90, rotate=0)

def print_8x8(symbol, xStartpoint):
    x = xStartpoint # x & y define startpoints of matrixes 
    y = 0
    with canvas(device) as draw:
        for value in symbol:
            byte = int(value,2) # parse to byte
            for j in range(8):  # go through each bit of byte
                if byte & 0x01 > 0:  # if byte is 1 draw point/turn on led
                    draw.point((x, y + j), fill="white")
                byte >>= 1
            x += 1
    
def print_16x16(symbol):
    with canvas(device) as draw:
        x, y = 0,0
        for value in symbol[0]:
            byte = int(value,2)
            for j in range(8):
                if byte & 0x01 > 0:
                    draw.point((x, y + j), fill="white")
                byte >>= 1
            x += 1
            
        x = 8
        y = 0
        for value in symbol[1]:
            byte = int(value,2)
            for j in range(8):
                if byte & 0x01 > 0:
                    draw.point((x, y + j), fill="white")
                byte >>= 1
            x += 1
            
        x = 16
        y = 0
        for value in symbol[2]:
            byte = int(value,2)
            for j in range(8):
                if byte & 0x01 > 0:
                    draw.point((x, y + j), fill="white")
                byte >>= 1
            x += 1
            
        x = 24
        y = 0
        for value in symbol[3]:
            byte = int(value,2)
            for j in range(8):
                if byte & 0x01 > 0:
                    draw.point((x, y + j), fill="white")
                byte >>= 1
            x += 1


