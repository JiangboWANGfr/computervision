#!/usr/bin/expect

set timeout -1
spawn sudo su -
expect ":"

#xiamian de ubuntu shi mima
send "ubuntu4\r"

expect "#"
send "chmod 777 /dev/ttyUSB0\n"

#xiamian shuru watchdog de jueduilujing
send "cd /home/ubuntu4/Documents/ComputerVision\n"
send "./EXEwatch.sh\n"

expect eof
