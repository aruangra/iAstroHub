psid=`ps axww | grep -i "skychart" | awk '{print $1}'`
sudo kill -9 $psid
