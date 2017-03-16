psid=`ps axww | grep -i "ddserver" | awk '{print $1}'`
sudo kill -9 $psid
