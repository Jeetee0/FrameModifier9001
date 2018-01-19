import argparse
import frame_reader
import frame_printer


def disco(args):

    # defining variables for printing
    path="txtFiles/" + args.filename
    frameList = frame_reader.read_frame_list(path)
    mirror = args.mirror
    cycles = args.cycles

    # if delay was not given in arguments the value from file is used.
    if args.delay == 0:
        delay = frame_reader.get_delay_from_file(path)
    else:
        delay = float(args.delay)/1000

    dimensions = frame_reader.get_size_of_frame(path)
    dimensionList = dimensions.split("x")
    width = int(dimensionList[0])
    height = int(dimensionList[1])

    if not args.msg == "":
        frame_printer.scrollText(args.msg)

    # start printing
    if mirror:
        print("printing now on {}x{} LED's with delay: {}").format(width*2, height*2, delay)
    else:
        print("printing now on {}x{} LED's with delay: {}").format(width, height, delay)

    for n in range(cycles):
        print ("round: {}/{}").format(n+1,cycles)
        frame_printer.print_frameList(frameList, width, height, delay, mirror)

def parseArguments():
    # create argument parser
    parser = argparse.ArgumentParser()

    parser.add_argument("--cycles", default=1, type=int, help="define how often the process should be running (0 is endless)")
    parser.add_argument("--delay", default=0, type=float, help="setting delay between frames (in ms)")
    parser.add_argument("--filename", default="frames.txt", type=str, help="choose .txt file to print")
    parser.add_argument("--mirror", default=False, type=bool, help="activates mirror mode, so your frame gets quadrupled")
    parser.add_argument("--msg", default="", type=str, help="message that gets displayed before printing pattern")

    args = parser.parse_args()
    print ("arguments: {}").format(args)
    return args

if __name__ == '__main__':
    args = parseArguments()
    disco(args)