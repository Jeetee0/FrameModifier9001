import frame_reader
import frame_printer

filename="txtFiles/frames.txt"

delay = frame_reader.get_delay_from_file(filename)
dimensions = frame_reader.get_size_of_frame(filename)
frameList = frame_reader.read_frame_list(filename)

print("printing now on {} LED's with delay: {}").format(dimensions, delay)
frame_printer.print_frameList(frameList, dimensions, delay, True)


# for x in range(0, 32):
#     for y in range (0, 8):
#         with canvas(device) as draw:
#             #print ("x: {}, y: {}".format(x,y))
#             draw.point((x, y), fill="white")
#             time.sleep(0.001)
#
# for y in range(0, 8):
#     for x in range (0, 32):
#         with canvas(device) as draw:
#             #print ("x: {}, y: {}".format(x,y))
#             draw.point((x, y), fill="white")
#             time.sleep(0.001)
#
#
# while (True):
#     y = random.randint(0,8)
#     for x in range (0, 32):
#         with canvas(device) as draw:
#             #print ("x: {}, y: {}".format(x,y))
#             draw.point((x, y), fill="white")
#             time.sleep(0.001)