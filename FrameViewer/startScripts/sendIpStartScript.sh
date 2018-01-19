#!/bin/bash
cd /home/pi/startScripts/

# remove ipaddress.txt if exists
[ -e ipaddress.txt ] && rm ipaddress.txt

STATE="error";

# loop until wifi connection is established
while [ $STATE == "error"       ]
do
        sleep 2
        STATE=$(ping -q -w 1 -c 1 `ip r | grep default | cut -d ' ' -f 3` > /dev/null && echo ok || echo error)
done

# get ip address
ifconfig wlan0 | grep "inet " | awk -F'[: ]+' '{ print $3 }' >> ipaddress.txt

# execute python script to send ip to server
python send_ip_address.py