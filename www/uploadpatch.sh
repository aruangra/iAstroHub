sudo mount /dev/sda1 /mnt/
sudo chmod 777 /mnt/* -R
sudo cp -rf /mnt/* /home/pi/

file="/mnt/patch.sh"
if [ -f "$file" ]; then
sudo sh /home/pi/patch.sh
sudo rm /home/pi/patch.sh
fi

sudo umount /mnt/
