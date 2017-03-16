#!/bin/bash
dev=`gphoto2 --auto-detect | grep usb | cut -b 36-42 | sed 's/,/\//'`
echo ${dev}
echo ${#dev}
if [ ${#dev} -ge 3 ]; then
echo loading > /home/pi/www/status_loading
echo Downloading... >> /home/pi/www/photo.txt
sudo gphoto2 -L > /home/pi/www/list.txt
i=0
while [ $i = 0 ]; do
tail -1 /home/pi/www/list.txt > /home/pi/www/tail.txt
while read line
do
echo $line
echo ${line:${#line}-4:4}
if [ ${line:${#line}-4:4} = "jpeg" ]; then
        i=1
        photo=${line:1:5}
        echo $photo
	    sudo gphoto2 --force-overwrite -p=$photo
	    cp *.JPG /home/pi/www/images/
	    cp *.JPG /home/pi/www/image.jpg
	    rm *.JPG
	    echo Downloaded. >> /home/pi/www/photo.txt
else
        sed '$d' < /home/pi/www/list.txt > /home/pi/www/tempfile
        mv /home/pi/www/list.txt /home/pi/www/list.txt.old
        mv /home/pi/www/tempfile /home/pi/www/list.txt
fi
done < /home/pi/www/tail.txt

filesize=`stat -c %s /home/pi/www/list.txt`
if [ $filesize = 0 ];then
	i=1
	echo No JPG file! >> /home/pi/www/photo.txt
fi

done
rm /home/pi/www/status_loading
fi
