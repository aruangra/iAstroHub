psid=`ps axww | grep -i "QSIcontrol" | awk '{print $1}'`
sudo kill $psid
