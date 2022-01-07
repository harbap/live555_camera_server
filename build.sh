#!/bin/bash

make
echo "make success !"
sleep 1

echo "copy files"

cp rtsp_server_arm target/
cp /workdir/openssl/libc2.23/lib/*so* target/
cp /workdir/x264lib/lib/lib/*so* target/
echo "update neuvition version packet"
rm /mnt/hgfs/share/neuvition-arm/version/neuvition/live555 -rf
cp target/ /mnt/hgfs/share/neuvition-arm/version/neuvition/live555 -rf

echo "everything is ok!!"
