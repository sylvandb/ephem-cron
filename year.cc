// ephemyear.cc: produce a year's worth of rises and sets suitable for plotting
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
// Sun Oct 10 1999 

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

#include "ephem.h"

int main (int argc, char *argv[])
{
  if (argc != 1 && argc != 4)
    {
      printf("usage: %s <lat(deg) long(deg) zone(hrs)>\n", argv[0]);
      exit(1);
    }

  float latitude;
  float longitude;
  float timezone;

  if (argc > 1) latitude = atof (argv[1]);
  else latitude = 42.3778343;

  if (argc > 2) longitude = atof (argv[2]);
  else longitude = -71.1063232;

  if (argc > 3) timezone = atof (argv[3]);
  else timezone = -5;

  time_t wallclock;  time(&wallclock);
  struct tm *tmp = localtime(&wallclock);
  tmp->tm_mon = 0;
  tmp->tm_mday = 1;
  tmp->tm_yday = 0;
  tmp->tm_hour = 12; 
  
  time_t tick = mktime(tmp);

  for (int day=0; day<365; day++)
    {
      wallclock = tick + 86400*day;
      struct tm *tmp = localtime(&wallclock);

      int Y = tmp->tm_year+1900;
      int M = tmp->tm_mon+1;
      int D = tmp->tm_mday; 

      //H = ((float) tmp->tm_gmtoff) / 3600.0;

      int risep;
      float rise;
      float riseaz;
      int setp;
      float set;
      float setaz;
      int allday;
      
      riseset(latitude, longitude, timezone, Y, M, D, 
	      risep, rise, riseaz, setp, set, setaz, allday);
      
//       fprintf(stderr,"%3d %3d %3d %3d %f %f\n", 
// 	      tmp->tm_yday, risep, setp, allday,
// 	      rise, set);

//       if (tmp->tm_isdst)		// deal with daylight savings time
// 	{
// 	  rise += 1;
// 	  set += 1;
// 	}
      
      if (risep)
 	{
	  if (rise > 22.0) rise -= 24; // catch unsightly wraparounds 
 	  printf("%05.02f\t",rise);
 	}
      else
 	{
	  if (allday == 1)
	    printf("00.00\t");
	  else
	    printf("12.00\t");
 	}
      
      if (setp)
  	{
	  if (set < 2.0) set += 24; // catch unsightly wraparounds
 	  printf("%05.02f\n",set);
  	}
      else
  	{
	  if (allday == 1)
	    printf("24.00\n");
	  else
	    printf("12.00\n");
  	}
    }
}

