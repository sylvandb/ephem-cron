#include <cstdio>
#include <cmath>
#include <cstdlib>
#include <ctime>
#include <cctype>

#include "x10ephem.h"

int main (int argc, char *argv[])
{
  if (argc != 1 && argc != 4)
    {
      printf("usage: %s <lat(deg) long(deg) zone(hrs)>\n", argv[0]);
      exit(1);
    }

  float latitude;
  float longitude;
  float zone;

  if (argc > 1) latitude = atof (argv[1]);
  else latitude = 42.3778343;

  if (argc > 2) longitude = atof (argv[2]);
  else longitude = -71.1063232;

  if (argc > 3)
    {
      if (argv[3][0] == '-' || isdigit(argv[3][0]))
	{
	  zone = atof (argv[3]);
	}
      else
	{
	  char buf[1024];
	  sprintf(buf,"TZ=%s",argv[3]);
	  putenv(buf);
	  tzset();
	  zone = -((float)timezone)/3600.0;
	}
    }
  else 
    {
      tzset();
      zone = -((float)timezone)/3600.0;
    }

  printf("Lat %f, Long %f, timezone %f\n", latitude, longitude, zone);
  
  time_t wallclock;  time(&wallclock);
  struct tm *tmp = localtime(&wallclock);
  int Y = tmp->tm_year+1900;
  int M = tmp->tm_mon+1;
  int D = tmp->tm_mday; 
  printf("Year %d, Month %d, Day %d\n", 
	 Y, M, D);
	 
  int risep;
  float rise;
  float riseaz;
  int setp;
  float set;
  float setaz;
  int allday;

  riseset(latitude, longitude, zone,
	  Y, M, D, 
	  risep, rise, riseaz, setp, set, setaz, allday);
  
  if (tmp->tm_isdst)		// deal with daylight savings time
    {
      rise += 1;
      set += 1;
    }

  if (risep)
    {
      printf("Sunrise at %02d:%02d at %03.0f degrees\n",
	     (int)floor(rise), 
	     (int)((rise-floor(rise))*60),
	     riseaz);
    }
  else
    {
      printf("No sunrise today\n");
    }

  if (setp)
    {
      printf("Sunset at %02d:%02d at %03.0f degrees\n",
	     (int)floor(set), 
	     (int)((set-floor(set))*60),
	     setaz);
    }
  else
    {
      printf("No sunset today\n");
    }
}

