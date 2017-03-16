psid1=`ps axww | grep -i "lin_guider" | awk '{print $1}'`
sudo kill -9 $psid1
psid2=`ps axww | grep -i "gtkImager" | awk '{print $1}'`
sudo kill -9 $psid2
