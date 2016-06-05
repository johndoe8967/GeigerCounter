# GeigerCounter
[![Build Status](https://travis-ci.org/johndoe8967/GeigerCounter.svg?branch=master)](https://travis-ci.org/johndoe8967/GeigerCounter)
ESP8266 based geiger counter project

## Summary
* count radioactive decay events on IO pin using interrupt
* calculates CPM, dose rate [uSv/h] 
* send data to ThingSpeak.com and RadMon.org
* telnet commandline to adjust application settings
* mobile (WIfi accesspoint) and stationary (WIFI client) mode 

## Additional needed software
uses Sming framework https://github.com/SmingHub/Sming
