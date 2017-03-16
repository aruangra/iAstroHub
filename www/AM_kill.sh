psid=`ps axww | grep -i "solve-field" | awk '{print $1}'`
sudo kill $psid
