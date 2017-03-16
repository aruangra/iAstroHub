#!/bin/bash
psid=`ps axww | grep -i "dslrcounter" | awk '{print $1}'`
sudo kill $psid

