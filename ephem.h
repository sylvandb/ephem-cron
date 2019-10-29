// ephem.h: interface spec for ephem.cc
// Copyright (C) 1999 "Christpher R. Wren" <c@drwren.com>
// 
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

// by "Christpher R. Wren" <c@drwren.com>
// August 5 1999


float parseTimezone(int argc,char **argv);


extern void
sunpos(float T,			// Julian day
       float TT,		// fractional centuries since 1900
       float &A5,		// azimuth return
       float &D5);		// declination return

extern void 
riseset(float B5,		// Latitude +N -S
	float L5,		// Longitude +E -W
	float H, 		// Time zone
	int Y,			// year
	int M,			// month
	int D,			// day
	int &risep,		// is there a sunrise?
	float &rise,		// time of sunrise
	float &riseaz,		// azimuth of sunrise
	int &setp,		// is there a sunset?
	float &set,		// time of sunset
	float &setaz,		// azimuth of sunset
	int &allday);		// is the sun up or down all day?
				//  0 = no
				//  1 = up
				// -1 = down
