psid=`ps axww | grep -i "gphoto2" | awk '{print $1}'`
sudo kill $psid
sudo gphoto2 --set-config bulb=0

psid=`ps axww | grep -i "dslrcounter" | awk '{print $1}'`
sudo kill $psid

psid=`ps axww | grep -i "shutter" | awk '{print $1}'`
sudo kill $psid

psid=`ps axww | grep -i "dslr_getphoto" | awk '{print $1}'`
sudo kill $psid

psid=`ps axww | grep -i "dslr" | awk '{print $1}'`
sudo kill $psid
