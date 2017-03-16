#!/bin/bash

# Modified for Pentax K-5 by Pierluigi Vittori
# Uses pktriggercord instead of gphoto2

sudo /home/pi/www/./dslrcounter $1 $2 $3 &
let "Exp = $3-1"

outfile=/home/pi/www/images/${5}-${6}-${3}s.pef

echo loading > /home/pi/www/status_loading

# Modified by Zachary
# When exposure is 0, the program will not set any exposure time but use the setting from the camera.
# This function is to capture flat, dark-flat and offset frames.
if [ $3 == 0 ]
then 
	sudo pktriggercord-cli --timeout 5 -i ${4} > $outfile
fi

if [ $3 != 0 ]
then
	sudo pktriggercord-cli --timeout 5 -t ${3} -i ${4} > $outfile
fi

sleep 5
echo Downloading... >> /home/pi/www/photo.txt
sudo ufraw-batch --embedded-image $outfile

echo Downloaded. >> /home/pi/www/photo.txt
rm /home/pi/www/status_loading
exit 0
