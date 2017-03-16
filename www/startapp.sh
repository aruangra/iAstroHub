cat /home/pi/www/CCD_log.txt >> /home/pi/www/CCD_log.bak
su root -c "DISPLAY=:1 /home/pi/lin_guider_pack/lin_guider/./lin_guider -geometry 800x600+0+0 > /dev/null 2>&1 &"
su root -c "DISPLAY=:2 /home/pi/OpenSkyImager/build/gtk/./gtkImager -f > CCD_log.txt 2>&1 &"
su root -c "DISPLAY=:3 GoQat -c /home/pi/www/Commands.txt > /dev/null 2>&1 &"
su root -c "python /home/pi/indi/indiwebmanager/servermanager/drivermanager.py &"
su root -c "sh /home/pi/indi/pyindi-ws.sh &"
