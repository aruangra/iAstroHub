#!/bin/bash
sudo gphoto2 --set-config capturetarget=1 > /home/pi/www/gphoto_log.txt 2>&1
sudo /home/pi/www/./dslrcounter $1 $2 $3 &
sudo gphoto2 -B$3 -F1 --capture-image > /home/pi/www/gphoto_log.txt 2>&1

line=`head -n 1 /home/pi/www/gphoto_log.txt`
check="`expr substr "$line" 1 3`"
if [ "$check" = "***" ]; then
	echo Check the camera! >> /home/pi/www/photo.txt

	psid=`ps axww | grep -i "gphoto2" | awk '{print $1}'`
	sudo kill $psid

	psid=`ps axww | grep -i "dslrcounter" | awk '{print $1}'`
	sudo kill $psid

	psid=`ps axww | grep -i "dslr" | awk '{print $1}'`
	sudo kill $psid

	if [ -e "/home/pi/www/status_dslr" ]
	then
		rm /home/pi/www/status_dslr
	fi

	if [ -e "/home/pi/www/status_loading" ]
	then
		rm /home/pi/www/status_loading
	fi

fi


