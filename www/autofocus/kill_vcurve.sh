psid=`ps axww | grep -i "move_in" | awk '{print $1}'`
sudo kill $psid

psid=`ps axww | grep -i "move_out" | awk '{print $1}'`
sudo kill $psid

psid=`ps axww | grep -i "move_to" | awk '{print $1}'`
sudo kill $psid

psid=`ps axww | grep -i "vcurve" | awk '{print $1}'`
sudo kill $psid