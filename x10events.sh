#!/bin/sh

exec 1> /dev/null
exec 2> /dev/null

export VISUAL=/usr/local/bin/x10events
export LAT=42.3778343 
export LON=-71.1063232 
export TOFF=-5

/usr/bin/crontab -u x10 -e 

