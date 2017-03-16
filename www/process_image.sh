echo processing > /home/pi/www/status_processing
sleep 1s
cp /home/pi/www/tmp_images/*.fit /home/pi/www/images/.
cd /home/pi/www/tmp_images/
ls *.fit > /home/pi/www/name.txt
fname=`cat /home/pi/www/name.txt`
cd /home/pi/www/
sleep 1s
echo LOADFILE:/home/pi/www/tmp_images/$fname > /tmp/gtkImager
sleep 1s
/home/pi/www/./load_check
histL=`cat /home/pi/www/hist_L.txt`
histU=`cat /home/pi/www/hist_U.txt`
echo MINADU:$histL > /tmp/gtkImager
sleep 1s
echo MAXADU:$histU > /tmp/gtkImager
sleep 2s
/home/pi/www/./minmaxadu_check
echo GETPREVIEW: > /tmp/gtkImager
sleep 1s
/home/pi/www/./hist_check

sed -i '$s/....$//' /home/pi/www/name.txt
fname=`cat /home/pi/www/name.txt`
cp /tmp/gtkImager.jpg /home/pi/www/images/$fname.jpg
cp /tmp/gtkImager.txt /home/pi/www/histogram.txt
perl /home/pi/www/histogram_split.pl
rm /home/pi/www/tmp_images/*.fit
rm /home/pi/www/status_processing
