psid=`ps axww | grep -i "indi_simple_html_cherrypy_server" | awk '{print $1}'`
sudo kill $psid
