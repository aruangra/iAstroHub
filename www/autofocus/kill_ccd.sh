psid=`ps axww | grep -i "ccdcontrol" | awk '{print $1}'`
sudo kill $psid
