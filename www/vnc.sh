su root -c "x11vnc -rfbport 5051 -forever -display :1 &"
su root -c "x11vnc -rfbport 5052 -forever -display :2 &"
su root -c "x11vnc -rfbport 5053 -forever -display :3 &"
sleep 5s
su root -c "/home/pi/www/noVNC/utils/./launch.sh --vnc localhost:5051 --listen 5551 &"
su root -c "/home/pi/www/noVNC/utils/./launch.sh --vnc localhost:5052 --listen 5552 &"
su root -c "/home/pi/www/noVNC/utils/./launch.sh --vnc localhost:5053 --listen 5553 &"
