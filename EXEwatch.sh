#!/bin/sh

### BEGIN INIT INFO
# Provides:          ComputerVision
# Required-Start:    $remote_fs $syslog
# Required-Stop:     $remote_fs $syslog
# Default-Start:     2 3 4 5
# Default-Stop:      0 1 6
# Short-Description: start ComputerVision
# Description:       start ComputerVision
### END INIT INFO


cd $(dirname $0)
xmake f -p linux -a x86_64 -m release
xmake

while true
do
	ps -ef | grep "CV" | grep -v "grep"
if [ "$?" -eq 1 ]
then
	xmake r #启动应用，修改成用户启动应用脚本或命令
	echo "process has been restarted!"
else
	echo "process already started!"
fi
	sleep 1
done
