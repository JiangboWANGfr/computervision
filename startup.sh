###
 # @Descripttion: 
 # @version: 
 # @Author: zzdr
 # @Date: 2019-12-27 21:28:01
 # @LastEditors  : zzdr
 # @LastEditTime : 2019-12-27 21:28:02
 ###
#!/usr/bin/expect

set timeout -1
spawn sudo su -
expect ":"

#xiamian de ubuntu shi mima
send "ubuntu\r"

expect "#"
send "chmod 777 /dev/ttyUSB0\n"

#xiamian shuru watchdog de jueduilujing
send "cd /home/ubuntu/Documents/ComputerVision\n"
send "./EXEwatch.sh\n"

expect eof
