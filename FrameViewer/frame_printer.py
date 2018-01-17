import time

from luma.led_matrix.device import max7219
from luma.core.interface.serial import spi, noop
from luma.core.render import canvas
from luma.core.virtual import viewport
from luma.core.legacy import text, show_message
from luma.core.legacy.font import proportional, CP437_FONT, TINY_FONT, SINCLAIR_FONT, LCD_FONT

# create matrix device
serial = spi(port=0, device=0, gpio=noop())
device = max7219(serial, cascaded=16, block_orientation=-90, rotate=0)
deviceText = max7219(serial, cascaded=16, block_orientation=-90, rotate=0)


def scrollText(message):
    show_message(deviceText, message, fill="white", font=proportional(LCD_FONT), scroll_delay=0.1)


def print_frameList(frameList, width, height, delay, mirror):
    if mirror:
        printWithMirror(frameList, delay, width*2, height*2)
    else:
        # looping through each frame
        for frame in frameList:
            x = 0
            # loop is used to draw all points and print them when loop reached end
            with canvas(device) as draw:
                # get each row
                for row in frame:
                    byte = int(row, 2)
                    # set column that gets defined by one byte (8bit)
                    for y in range(height):
                        #print ("x:{}, y:{}").format(x,y)
                        if byte & 0x01 > 0:  # if byte is 1 draw point/turn on led
                            point = get_point_for_chained_matrixes(x,y)
                            draw.point(point, fill="white")
                        byte >>= 1
                    x += 1
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
                        point = get_point_for_chained_matrixes(x,y)
                        draw.point(point, fill="white")
                        point = get_point_for_chained_matrixes(width-x-1, y)
                        draw.point(point, fill="white")
                        point = get_point_for_chained_matrixes(x, height-y-1)
                        draw.point(point, fill="white")
                        point = get_point_for_chained_matrixes(width-x-1, height-y-1)
                        draw.point(point, fill="white")
                    byte >>= 1
                y += 1
        time.sleep(delay)



# because i cant control the matrixes like an quadratical array i need to convert the x,y-values
# controlling like this:          transferring into:
# 0            x           31     0      x                31                      63                      95                      127
#   ##### ##### ##### #####   |     ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
#   # 1 # # 2 # # 3 # # 4 #   |   y # 1 # # 2 # # 3 # # 4 # # 5 # # 6 # # 7 # # 8 # # 9 # # 10# # 11# # 12# # 13# # 14# # 15# # 16#
#   ##### ##### ##### #####   |     ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### ##### #####
#   ##### ##### ##### #####   |   7
#   # 5 # # 6 # # 7 # # 8 #   |
# y ##### ##### ##### #####   |
#   ##### ##### ##### #####   |
#   # 9 # # 10# # 11# # 12#   |
#   ##### ##### ##### #####   |
#   ##### ##### ##### #####   |
#   # 13# # 14# # 15# # 16#   |
#32 ##### ##### ##### #####   |

def get_point_for_chained_matrixes(x, y):
    while y > 7:
        y -= 8
        x += 32
    return (x,y)