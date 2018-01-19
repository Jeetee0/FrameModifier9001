from luma.led_matrix.device import max7219
from luma.core.interface.serial import spi, noop
from luma.core.legacy import text, show_message
from luma.core.legacy.font import proportional, CP437_FONT, TINY_FONT, SINCLAIR_FONT, LCD_FONT

import os

# read file
file = open('ipaddress.txt', 'r')
ip = file.readline()

# send ip address of raspberry pi to youris htw server
os.system("curl --data '' http://141.45.92.235:8080/guessMyMusic/ip/" + ip)


# print ip address to led matrixes
serial = spi(port=0, device=0, gpio=noop())
deviceText = max7219(serial, cascaded=4, block_orientation=-90, rotate=0)

msg = "ip: " + ip
show_message(deviceText, msg, fill="white", font=proportional(LCD_FONT), scroll_delay=0.1)