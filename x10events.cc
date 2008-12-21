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
// comments should be # {token} [offset] [variance]
// where
//   {token} is one of
//      "rise" for times relative to sunrise
//      "set" for times relative to sunset
//      "riseif before" conditional if time is before sunrise
//      "riseif after"  conditional if time is after sunrise
//      "setif before" conditional if time is before sunset
//      "setif after"  conditional if time is after sunset
// 	"abs" for absolute times, see note below
//   offset specifies time relative to the token
//   variance specifies a bound for uniform noise added to the time 
// offset and variance may be specified as decimal hours or as [-]hh:mm
//
// special note:
//   If abs specifies an offset and variance, then the offset is the mean time.
//   If no arguments are specified for abs, then the time is as specified by
//   the hour and minute cron fields.
//
// for example:
// 
// 11 07 * * * /usr/local/bin/heyu turn tree on        #x10 rise
// 36 16 * * * /usr/local/bin/heyu turn tree off       #x10 set 0.1
// 04 16 * * * /usr/local/bin/heyu turn floorlamp on   #x10 set -0.50
// 00 00 * * * /usr/local/bin/heyu turn floorlamp off  #x10 abs
// 29 16 * * * /usr/local/bin/heyu turn office on      #x10 set 0.1
// 12 00 * * * /usr/local/bin/heyu turn office off     #x10 abs 00:30 0.5
// 00 08 * * * /usr/local/bin/heyu turn overhead on    #x10 riseif before


/**********************************************************
 * ToDo:
 *   resolve issue with crontab(5):
 *     "comments are not allowed on the same line as a command"
 */

#include <iostream>
#include <sstream>
#include <fstream>
#include <math.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include "ephem.h"

#define _def2str(x) #x
#define def2str(x) _def2str(x)

#define HOURMINS 60L
#define DAYMINS (24L * HOURMINS)

using namespace std;

void print_modifed(ostream *out, int minute, int hour, 
		   char* day, char* month, char* dow, 
		   char* command, char* tag,
		   char* mean, char* var)
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
  if (NULL!=mean)
    (*out) << " " << mean;
  else if (NULL!=var)
    (*out) << " 0";
  if (NULL!=var)
    (*out) << " " << var;
  (*out) << endl;
}

int wrap(int time)
{
  while (time < 0)        {time += DAYMINS;}
  while (time >= DAYMINS) {time -= DAYMINS;}
  return(time);
}

int noise(int base, int var)
{
  // deterministic
  if (var == 0) return(base);

  // uniform over [-variance.variance]
  var = abs(var);
  // make this calculation with integer math???
  int diff = lrint((2.0 * var * rand()) / RAND_MAX - var);
  return(wrap(base + diff));
}

int timeconv(char *time)
{
  if (time == (char*)NULL)
    return(0);

  if (!strchr(time, ':')) 
    {
      return lrint(atof(time)*HOURMINS);	// rework w/atoi()???
    }
  else
    {
      char* work = new char[strlen(time)+1];
      strcpy(work,time);

      int hour = atoi(strtok(work, ":"));	// get hour
      int min = atoi(strtok(NULL, ":"));	// get minutes
      delete [] work;

      if (hour>=0 && '-' != *time)	 // preserve sign even w/hours==0
	return (hour * HOURMINS + min);
      else
	return (hour * HOURMINS - min);
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
  sleep(1);	// relinquish my timeslice for better randomness later
  // this is stupid, but dynamic ostrstream doesnt seem to work
  // properly on large strings - arggg!!  JAVAJAVAJAVAJAVA
  //char *outbuf = new char[1000000];
  //ostrstream out(outbuf, 1000000);
  //  that's right, strstream is BROKEN!!!!  using stringstream instead.
  ostringstream out;

  istream *in = &cin;
  char *filename = (char*)NULL;

  double latitude  = DLAT;
  double longitude = DLON;
  double timezone;

  if (getenv("LAT") != NULL)
    latitude = atof(getenv("LAT"));

  if (getenv("LON") != NULL)
    longitude = atof(getenv("LON"));

  if (getenv("TOFF") != NULL) {
    char *fakeargv[5]={NULL};
    fakeargv[3]=getenv("TOFF");
    timezone = (double)parseTimezone(4,fakeargv);
  } else
    timezone = (double)parseTimezone(0,NULL);
  
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
  float frise;
  float riseaz;
  int setp;
  float fset;
  float setaz;
  int allday;
  
  riseset(latitude, longitude, timezone,
	  Y, M, D,
	  risep, frise, riseaz, setp, fset, setaz, allday);
  
  int rise=lrint(frise*HOURMINS);
  int set=lrint(fset*HOURMINS);

  if (tmp->tm_isdst > 0)		// deal with daylight savings time
    {
      rise += HOURMINS;
      set += HOURMINS;
    }
  
  if (!risep)
    {
      rise = 0;
    }
  
  if (!setp)
    {
      set = DAYMINS;
    }

  unsigned int buflen = 256;
  char *buf = new char[buflen];

  
  while(! in->eof())
    {
      in->getline(buf, buflen, '\n');
      if (buf[0] == '#' || !in->eof() && !strstr(buf,"#x10"))
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

	  // split line into two parts to prevent crashing on some comment lines
	  char *cronentry = strtok(buf,"#");
	  char *x10tail = strtok((char *) NULL, "\0");
	  char *side;

	  // process cron part
	  minute = strtok(cronentry, " "); // minute
	  hour = strtok((char *) NULL, " "); // hour

	  day =     strtok((char *) NULL, " "); // day of month
	  month =   strtok((char *) NULL, " "); // month
	  dow =     strtok((char *) NULL, " "); // day of week

	  command = strtok((char *) NULL, "#"); // command

	  // process x10tail
	  token = strtok(x10tail, " "); // pass by the "#x10 " to the real token

	  if (!strcmp(token,"x10#")) {	// x10 conditional that was false the last time
		  // process cron part previously commented out
		  minute = strtok((char *) NULL, " "); // minute
		  hour = strtok((char *) NULL, " "); // hour

		  day =     strtok((char *) NULL, " "); // day of month
		  month =   strtok((char *) NULL, " "); // month
		  dow =     strtok((char *) NULL, " "); // day of week

		  command = strtok((char *) NULL, "#"); // command

		  // process the real x10tail
		  token = strtok((char *) NULL, " "); // pass by the "#x10 " to the real token
	  }

	  token = strtok((char *) NULL, " "); // riseif,setif,rise,set,abs

  	  if (command != (char *) NULL)
	    {
	      int special = 0;	  
	      char* tag=NULL;
	      char tagspace[30];	// room to spare
	      bool conditional = false;
	      
	      if (!strcmp(token, "riseif"))
		{
		  conditional = true;
		  tag = "#x10 riseif ";
		  special = atoi(hour)*HOURMINS + atoi(minute);
		}
	      if (!strcmp(token, "setif"))
		{
		  conditional = true;
		  tag = "#x10 setif ";
		  special = atoi(hour)*HOURMINS + atoi(minute);
		}
	      
	      if (conditional)
	      {
		  side  = strtok((char *) NULL, " "); // before,after
	      }

	    offset = strtok((char *) NULL, " "); // offset
	    variance = strtok((char *) NULL, " "); // randomize
	    // finish strtok()'s above, because timeconv() uses strtok()
	    int mean = timeconv(offset);
	    int var = timeconv(variance);

	      if (conditional && (mean!=0 || var!=0))
	      {
		  special = 0;
	      }

	      if (!strcmp(token, "abs"))
		{
		  tag = "#x10 abs";

		  if ((mean==0) && (var==0))
		    {
		      special = atoi(hour)*HOURMINS + atoi(minute);
		    }
		}
	      if (!strcmp(token, "rise"))
		{
		  tag = "#x10 rise";
		  special = rise;
		}
	      if (!strcmp(token, "set"))
		{
		  tag = "#x10 set";
		  special = set;
		}


	    if (!tag)
	    {
		// space ("# x10") prevents future processing
		strcpy(tagspace,"# x10 unknown: ");
		tag=tagspace;
		strcat(tag,token);
	    }


	    // add in the random element
	    int time = noise( wrap(special+mean), var);

	    if (conditional)
	    {
		strcpy(tagspace,tag);
		tag=tagspace;

		if ('b'==*side)	// before
		{
		    strcat(tag,"before");
		    if (time > ('r'==*token?rise:set)) // time not before
  			out << " #x10# ";
		}
		else		// after
		{
		    strcat(tag,"after");
		    if (time < ('r'==*token?rise:set)) // time not after
  			out << " #x10# ";
		}
	    }

	    int hour   = (int) (time / HOURMINS);
	    int minute = (int) (time % HOURMINS);
	    //out << " #x10 special: " << special << "  time: " << time << endl;

	    print_modifed(&out, minute, hour, 
			  day, month, dow,
			  command, tag,
			  offset, variance);
	  }
	}
    }

  // add my special ending tag lines
  out << " #x10 operands:  {rise|set|abs|riseif|setif} [before|after] [offset [variance]]" << endl;
  out << " #x10 " def2str(VERSION) " processed " << Y;
  out.width(2); out.fill('0'); out << M;
  out.width(2); out.fill('0'); out << D << "-";
  out.width(2); out.fill('0'); out << tmp->tm_hour;
  out.width(2); out.fill('0'); out << tmp->tm_min << ", sunrise=";
  out.width(2); out.fill('0'); out << rise/HOURMINS << ":";
  out.width(2); out.fill('0'); out << rise%HOURMINS << ", sunset=";
  out.width(2); out.fill('0'); out << set/HOURMINS << ":";
  out.width(2); out.fill('0'); out << set%HOURMINS << endl;

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
