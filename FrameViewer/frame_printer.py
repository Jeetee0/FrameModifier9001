import time

from luma.led_matrix.device import max7219
from luma.core.interface.serial import spi, noop
from luma.core.render import canvas
from luma.core.virtual import viewport
from luma.core.legacy import text, show_message
from luma.core.legacy.font import proportional, CP437_FONT, TINY_FONT, SINCLAIR_FONT, LCD_FONT

# create matrix device
serial = spi(port=0, device=0, gpio=noop())
device = max7219(serial, cascaded=16, block_orientation=90, rotate=0)
deviceText = max7219(serial, cascaded=2, block_orientation=90, rotate=0)


def printText(message):
    show_message(deviceText, message, fill="white", font=proportional(CP437_FONT))


def print_frameList(frameList, dimensions, delay, mirror):
    dimensionList = dimensions.split("x")
    width = int(dimensionList[0])
    height = int(dimensionList[1])

    if mirror:
        printWithMirror(frameList, delay, width*2, height*2)
    else:
        # looping through each frame
        for frame in frameList:

            # loop is used to draw all points and print them when loop reached end
            with canvas(device) as draw:
                y = 0
                # get each row
                for row in frame:
                    byte = int(row, 2)

                    # set column that gets defined by one byte (8bit)
                    for x in range(height):
                        # print ("x:{}, y:{}").format(x,y)
                        if byte & 0x01 > 0:  # if byte is 1 draw point/turn on led
                            draw.point((x, y), fill="white")
                        byte >>= 1
                    y += 1
            time.sleep(delay)

def printWithMirror(frameList, delay, width, height):
    # looping through each frame
    for frame in frameList:

        # loop is used to draw all points and print them when loop reached end
        with canvas(device) as draw:
            y = 0
            # get each row
            for row in frame:
                byte = int(row, 2)

                # set column that gets defined by one byte (8bit)
                for x in range(height):
                    # print ("x:{}, y:{}").format(x,y)
                    if byte & 0x01 > 0:  # if byte is 1 draw point/turn on led
                        draw.point((x, y), fill="white")
                        draw.point((width-x-1, y), fill="white")
                        draw.point((x, height-y-1), fill="white")
                        draw.point((width-x-1, height-y-1), fill="white")
                    byte >>= 1
                y += 1
        time.sleep(delay)