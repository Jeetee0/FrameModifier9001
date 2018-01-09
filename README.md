# FrameModifier9001
This Program was written for the lecture "Vertiefung Programmieren" in 5.Semester at the HTW Berlin. 
It was created with Qt creator using Qt 5.10.0 MinGW 32bit.

With the program you can easily design patterns that can then transferred to four "max7219 8x8LED matrixes". 
In my example I am using 2x2 matrixes, resulting in a 16x16 LED matrix.


You can export your project to get a frames.txt file that can then interpreted by my python library on a raspberry pi.
For that you need the SPI kernel driver enabled. Also you will need the luma.led_matrix library.
Best is to follow this guide to get the neccessary libraries: https://luma-led-matrix.readthedocs.io/en/latest/install.html

You can then clone my git repo and play with the code: https://github.com/Jeetee0/FrameModifier9001.git 
Keep in mind that the FrameCreator is designed for four matrixes in total.
