import ephem
import os

RA = [line.strip() for line in open('/home/pi/www/AM_RA.txt')]
RA_J2000 = RA[0]+':'+RA[1]+':'+RA[2]

DEC = [line.strip() for line in open('/home/pi/www/AM_DEC.txt')]
if DEC[0] == '1':
   DEC_J2000 = '+'+DEC[1]+':'+DEC[2]+':'+DEC[3]
else:
   DEC_J2000 = '-'+DEC[1]+':'+DEC[2]+':'+DEC[3]
   
center_J2000 = ephem.Equatorial(RA_J2000,DEC_J2000, epoch=ephem.J2000)
center_JNow = ephem.Equatorial(center_J2000, epoch=ephem.now())

center_JNow_ra=str(center_JNow.ra)
ra_all=center_JNow_ra.split(':')
ra_hh=ra_all[0]
ra_mm=ra_all[1]
ra_ss=str(int(round(float(ra_all[2]))))

f1=open('/home/pi/www/AM_RA_JNow.txt', 'w')
f1.write(ra_hh+'\n')
f1.write(ra_mm+'\n')
f1.write(ra_ss+'\n')
f1.close()

center_JNow_dec=str(center_JNow.dec)
dec_all=center_JNow_dec.split(':')

dec_dd=dec_all[0]

if dec_dd[:1]=='-':
   dec_dd=dec_dd[1:]
   dec_sign='-1'
else:
   dec_dd=dec_dd
   dec_sign='1'
   
dec_mm=dec_all[1]
dec_ss=str(int(round(float(dec_all[2]))))

f2=open('/home/pi/www/AM_DEC_JNow.txt', 'w')
f2.write(dec_sign+'\n')
f2.write(dec_dd+'\n')
f2.write(dec_mm+'\n')
f2.write(dec_ss+'\n')
f2.close()


LON=str(((float(ra_ss)/60+float(ra_mm))/60+float(ra_hh))*15)
LAT=str(((float(dec_ss)/60+float(dec_mm))/60+float(dec_dd))*float(dec_sign))

frame = [line.strip() for line in open('/home/pi/www/AM_frame.txt')]
width  = str(float(frame[0])/60)
height = str(float(frame[1])/60)
rotation = str(float(frame[2])*(-1))

FOV = str(float(frame[0])/60*2)


f1 = open('/home/pi/www/setting.skyset', 'r')
f2 = open('/home/pi/www/SkySafari.skyset', 'w')
for line in f1:
   parameter=line.split('=')
   if   parameter[0]== 'DisplayCoordSystem':
      f2.write('DisplayCoordSystem=1\n')   
   elif parameter[0]== 'DisplayCenterLon':
      f2.write('DisplayCenterLon=' + LON + '\n')
   elif parameter[0]== 'DisplayCenterLat':
      f2.write('DisplayCenterLat=' + LAT + '\n')
   elif parameter[0]== 'DisplayFOV':
      f2.write('DisplayFOV=' + FOV + '\n')
   elif parameter[0]== 'ScopeCameraFOVWidth':
      f2.write('ScopeCameraFOVWidth=' + width + '\n')
   elif parameter[0]== 'ScopeCameraFOVHeight':
      f2.write('ScopeCameraFOVHeight=' + height + '\n')
   elif parameter[0]== 'ScopeCameraFOVOn':
      f2.write('ScopeCameraFOVOn=1\n')
   elif parameter[0]== 'ScopeDrawWhenDisconnected':
        f2.write('ScopeDrawWhenDisconnected=1\n')
   elif parameter[0]== 'ScopeFieldRotation':
      f2.write('ScopeFieldRotation=' + rotation + '\n')
   else:
      f2.write(line)

f2.close()
f1.close()

os.system("cat /home/pi/www/SkySafari.header /home/pi/www/SkySafari.skyset > /home/pi/www/temp.skyset")
os.system("mv /home/pi/www/temp.skyset /home/pi/www/SkySafari.skyset")
