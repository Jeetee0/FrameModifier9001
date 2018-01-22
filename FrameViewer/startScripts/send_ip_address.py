import os

# read file
file = open('ipaddress.txt', 'r')
lineList = file.readlines()
ip = lineList[-1]
file.close()

print ip

# send ip address of raspberry pi to youris htw server
os.system("curl --data '' http://141.45.92.235:8080/guessMyMusic/ip/" + ip)