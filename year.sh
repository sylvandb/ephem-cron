#!/bin/sh

# ephemyear.sh: gnuplot wrapper around ephemyear.cc
# Copyright (C) 1999 "Christpher R. Wren" <c@drwren.com>
# 
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

# by "Christpher R. Wren" <c@drwren.com>
# Sun Oct 10 1999 

awk '{print $1}' > /tmp/rise.txt < riseset.mat
awk '{print $2}' > /tmp/set.txt <  riseset.mat

./year 42.3778343 -71.1063232 -5 | awk '{print $1}' > /tmp/myrise.txt
./year 42.3778343 -71.1063232 -5 | awk '{print $2}' > /tmp/myset.txt

gnuplot <<-EOF
	set terminal gif
	set title  "Calculated sunrise and sunset"
	set xlabel "Julian Day"
	set ylabel "Hour of Day"
	set xrange [0:365]
	set yrange [0:24]
	set data style lines
	plot "/tmp/myrise.txt" title "calculated rise", \
	     "/tmp/rise.txt"   title "official rise",   \
	     "/tmp/myset.txt"  title "calculated set",  \
	     "/tmp/set.txt"    title "official set"
	! xmessage exit
	EOF

rm /tmp/myrise.txt /tmp/myset.txt /tmp/rise.txt /tmp/set.txt
