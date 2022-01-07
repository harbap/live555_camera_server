#!/bin/bash 

rm /home/workdir/live555/start.log 

echo "[`date +%y%m%d-%H:%M:%S`]" >> /home/workdir/live555/start.log 
echo "runing live555 autosh.sh " >> /home/workdir/live555/start.log

pidof rtsp_server_arm

if [ $? -ne 0 ]
then
echo "start rtsp server..." >> /home/workdir/live555/start.log 
nohup /home/workdir/live555/rtsp_server_arm >/dev/null 2>&1 & 
else 
echo "rtsp server is already running..." >> /home/workdir/live555/start.log 
fi 


