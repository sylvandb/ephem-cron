// x10events.cc: adjust x10 crontab according to comments
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

// interpret crontab vomments and modify the entry accordingly
// comments should be # token [offset] [variance]
// where token is one of "abs" for absolute times
//                       "rise" for timesrelative to sunrise
//                       "set" for times relative to sunset
//       offset specifies time relative to the token
//       variance specifies a bound for uniform noise added to the time 
// offset and variance may be specified as decimal hours or as [-]hh:mm
//
// special case: if abs specifies an offset and variance, then the
// offset is the mean time.  If no arguments are specified for abs,
// then the time is that specified by the hour and minute cron fields.
//
// for example:
// 
// 11 07 * * * /usr/local/bin/heyu turn tree on        # rise
// 36 16 * * * /usr/local/bin/heyu turn tree off       # set 0.1
// 04 16 * * * /usr/local/bin/heyu turn floorlamp on   # set -0.50
// 00 00 * * * /usr/local/bin/heyu turn floorlamp off  # abs
// 29 16 * * * /usr/local/bin/heyu turn office on      # set 0.1
// 12 00 * * * /usr/local/bin/heyu turn office off     # abs 00:30 0.5

#include <iostream.h>
#include <strstream.h>
#include <fstream.h>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "ephem.h"

void print_modifed(ostream *out, int minute, int hour, 
		   char* day, char* month, char* dow, 
		   char* command, char* tag,
		   double fmean, char* mean, double fvar, char* var)
{
  out->width(2);
  out->fill('0');
  out->setf(ios::right);
  (*out) << minute << " ";
  
  out->width(2);
  out->fill('0');
  out->setf(ios::right);
  (*out) << hour << " ";

  (*out) << day;
  (*out) << " " << month;
  (*out) << " " << dow;
  (*out) << " " << command;
  (*out) << tag;
  if (fmean!=0.0 || fvar!=0.0)  (*out) << " " << mean;
  if (fvar!=0.0)  (*out) << " " << var;
  (*out) << endl;
}

double wrap(double time)
{
  while (time < 0.0)   {time = time + 24.0;}
  while (time >= 24.0) {time = time - 24.0;}
  return(time);
}

double noise(double base, double var)
{
  // deterministic
  if (var == 0.0) return(base);

  // uniform over [-variance.variance]
  var = fabs(var);
  double diff = 2.0 * var * ((double) rand() / RAND_MAX) - var;
  double time = base + diff;
  return(wrap(time));
}

double timeconv(char *time)
{
  if (time == (char*)NULL)
    return(0.0);

  if (!strchr(time, ':')) 
    {
      return atof(time);
    }
  else
    {
      char* work = new char[strlen(time)+1];
      strcpy(work,time);

      double hour = atof(strtok(work, ":"));	// get hour
      double min = atof(strtok(NULL, ":"));	// get seconds
      delete [] work;

      if (hour>=0.0)
	return (hour + (min/60.0));
      else
	return (hour - (min/60.0));
    }
}

void usage()
{
  cerr << "usage: x10events";
  cerr << " [crontab_file]";
  cerr << " [lat(deg) long(deg) zone(hrs)]";
  cerr << endl;
  exit(1);
}

int main (int argc, char *argv[])
{
  // this is stupid, but dynamic ostrstream doesnt seem to work
  // properly on large strings - arggg!!  JAVAJAVAJAVAJAVA
  char *outbuf = new char[1000000];
  ostrstream out(outbuf, 1000000);

  istream *in = &cin;
  char *filename = (char*)NULL;

  double latitude  = 42.3778343;;
  double longitude = -71.1063232;
  double timezone  = -5.0;

  if (getenv("LAT") != NULL)
    latitude = atof(getenv("LAT"));

  if (getenv("LON") != NULL)
    longitude = atof(getenv("LON"));

  if (getenv("TOFF") != NULL)
    timezone = atof(getenv("TOFF"));
  
  if ((argc!=1) && (argc!=2) && (argc!=4) && (argc!=5))
    {
      usage();
    }
  
  int arg = 1;
  if ((argc==2) || (argc==5))
    {
      filename = argv[arg];
      ifstream* fin = new ifstream(filename);
      if (!fin->is_open())
	{
	  cerr << "non-existant input file" << endl;
	  usage();
	}
      in = fin;
      arg++;
    }
  
  if ((arg==1) && (argc==4) ||
      (arg==2) && (argc==5))
    {
      char *endptr = NULL;

      latitude = strtod (argv[arg], &endptr);
      if (endptr == argv[arg]) usage();
      arg++;

      longitude = strtod (argv[arg], &endptr);
      if (endptr == argv[arg]) usage();
      arg++;

      timezone = strtod (argv[arg], &endptr);
      if (endptr == argv[arg]) usage();
      arg++;
    }
  
  time_t wallclock;  time(&wallclock);
  struct tm *tmp = localtime(&wallclock);

  int Y = tmp->tm_year+1900;
  int M = tmp->tm_mon+1;
  int D = tmp->tm_mday; 
  
  // Cheesy RNG initialization, but it works.
  srand(wallclock % 100000);

  int risep;
  float rise;
  float riseaz;
  int setp;
  float set;
  float setaz;
  int allday;
  
  riseset(latitude, longitude, timezone,
	  Y, M, D,
	  risep, rise, riseaz, setp, set, setaz, allday);
  
  if (tmp->tm_isdst)		// deal with daylight savings time
    {
      rise += 1;
      set += 1;
    }
  
  if (!risep)
    {
      rise = 00.00;
    }
  
  if (!setp)
    {
      set = 24.00;
    }

  unsigned int buflen = 256;
  char *buf = new char[buflen];

  
  while(! in->eof())
    {
      in->getline(buf, buflen, '\n');
      if (buf[0] == '#')
        {
	  out << buf << endl;
        }
      else
        {
	  char *command;
	  char *token;
	  char *offset;
	  char *variance;
	  char *minute;
	  char *hour;
	  char *day;
	  char *month;
	  char *dow;

	  minute = strtok(buf, " "); // minute
	  hour = strtok((char *) NULL, " "); // hour

	  day =     strtok((char *) NULL, " "); // day of month
	  month =   strtok((char *) NULL, " "); // month
	  dow =     strtok((char *) NULL, " "); // day of week

	  command = strtok((char *) NULL, "#"); // command

	  token = strtok((char *) NULL, " "); // rise,set,abs
	  offset = strtok((char *) NULL, " "); // offset
	  variance = strtok((char *) NULL, " "); // random 

	  double mean = timeconv(offset);
	  double var = timeconv(variance);

  	  if (command != (char *) NULL)
	    {
	      double special = 0.0;	  
	      char* tag;
	      
	      if (!strcmp(token, "abs"))
		{
		  tag = "# abs";

		  if ((mean==0.0) && (var==0.0))
		    {
		      special = (atof(hour) + atof(minute)/60.0);
		    }
		}
	      if (!strcmp(token, "rise"))
		{
		  tag = "# rise";
		  special = rise;
		}
	      if (!strcmp(token, "set"))
		{
		  tag = "# set";
		  special = set;
		}
	      
	    double time = wrap( noise( wrap(special+mean), var));
	    
	    int hour = (int) floor(time);
	    int minute = (int) ((time - hour) * 60.0);
	    
	    print_modifed(&out, minute, hour, 
			  day, month, dow,
			  command, tag,
			  mean, offset, var, variance);
	  }
	}
    }

  if (filename == (char*) NULL)
    {
      cout << out.str();
    }
  else
    {
      ((ifstream*) in)->close();
      sleep(2); // wait a bit so cron doesn't think the file is unchanged
      ofstream fout(filename, ios::out | ios::trunc);
      fout << out.str();
      fout.close();
    }
}
