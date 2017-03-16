psid1=`ps axww | grep -i "pagekite" | awk '{print $1}'`
sudo kill -9 $psid1
sudo cat /home/pi/www/pagekite_log.txt >> /home/pi/www/pagekite_bak.txt
sudo python /home/pi/pagekite.py 80 > /home/pi/www/pagekite_log.txt &
