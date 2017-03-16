psid=`ps axww | grep -i "pktriggercord-cli" | awk '{print $1}'`
sudo kill $psid

psid=`ps axww | grep -i "dslrcounter" | awk '{print $1}'`
sudo kill $psid

psid=`ps axww | grep -i "shutter" | awk '{print $1}'`
sudo kill $psid

psid=`ps axww | grep -i "dslr" | awk '{print $1}'`
sudo kill $psid
