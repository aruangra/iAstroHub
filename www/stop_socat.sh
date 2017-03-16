psid=`ps axww | grep -i "socat" | awk '{print $1}'`
sudo kill $psid
