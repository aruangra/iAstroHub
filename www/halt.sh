psid1=`ps axww | grep -i "lin_guider" | awk '{print $1}'`
sudo kill -SIGUSR1 $psid1
su root -c "shutdown -h now"
