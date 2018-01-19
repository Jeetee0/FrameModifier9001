import os

# send "bad" ip address of raspberry pi to youris htw server
os.system("curl --data '' http://141.45.92.235:8080/guessMyMusic/ip/raspberry_not_online")

# delete file
os.remove('ipaddress.txt')