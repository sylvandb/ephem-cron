// ephem.cc: calcute sunup and sundown anywhere in the world
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
//
// Ported from BASIC by c@drwren.com on August 5 1999
//
// http://www.skypub.com/resources/software/basic/programs/sunup.bas
// Original BASIC code by Roger W. Sinnott
// Sky & Telescope for August 1994, page 84.
//
// comments in that code claim accuracy to the minute within 
// several hundred years of 1994.
//
// latitude is positive North of the equator 
// longitude is positive East of the prime meridian 
// timezones are positive East of UTC.
//
// The original port contained the BASIC code in comments near the C
// code.  Due to copyright concerns, those comments have been replaced 
// by comments indicating line numbers in the original code.  Sorry.

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

#define P1 (M_PI)
#define P2 (2*P1)
#define DR (P1/180.0)
#define K1 (15*DR*1.0027379)
#define SGN(n) (n<0 ? -1 : 1)

void sunpos(float T, float TT, float &A5, float &D5)
{
  // line 910
  float L =.779072+.00273790931*T;

  // line 950
  float G=.993126+.0027377785*T;

  // line 960
  L=L- (int)L;
  G=G- (int)G;

  // line 970
  L=L*P2; 
  G=G*P2;

  // line 980
  float V=.39785*sin(L);

  // line 990
  V=V-.01000*sin(L-G);

  // line 1000
  V=V+.00333*sin(L+G);

  // line 1010
  V=V-.00021*TT*sin(L);

  // line 1020
  float U=1-.03349*cos(G);

  // line 1030
  U=U-.00014*cos(2*L);

  // line 1040
  U=U+.00008*cos(L);

  // line 1050
  float W=-.00010-.04129*sin(2*L);

  // line 1060
  W=W+.03211*sin(G);

  // line 1070
  W=W+.00104*sin(2*L-G);

  // line 1080
  W=W-.00035*sin(2*L+G);

  // line 1090
  W=W-.00008*TT*sin(G);

  // line 1100
  // line 1110 
  // line 1120 
  // This code claims to compute the sun's dclination and right ascention
  float S=W/sqrt(U-V*V);

  // line 1130
  A5=L+atan(S/sqrt(1-S*S));

  // line 1140
  S=V/sqrt(U);
  D5=atan(S/sqrt(1-S*S));

  // line 1150
  float R5=1.00021*sqrt(U);

  // line 1160
}

void 
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
	int &allday)		// is the sun up or down all day?
				//  0 = no
				//  1 = up
				// -1 = down
{
  float Aa[2],Da[2];

  setp = 1;
  risep = 1;
  allday = 0;

  L5=L5/360;
  float Z0=-H/24;               // original BASIC uses screwy H
				// I change this so H makes sense as a timezone

				// convert calendar date to julian day
  // line 1190                           gregorian
  int G=1;
  if (Y<1583) 
    G=0;
  
  // line 1200                           fractional day?
  int D1=D;
  float F = -.5;

  // line 1210                           (year + shifted fraction)
  int J=-(int) (7*((int) ((M+9)/12) + Y) / 4);

  int J3;

  // line 1220: gregorian correction
  if ( G!=0 )
    {

  // line 1230                           intrayear offset
      float S=SGN(M-9);
      float A=S*(M-9);

  // line 1240                           year + adjusted fraction
      J3=(int) (Y+S* (int) (A/7));

  // line 1250                           3/4 adjusted century 
      J3=-(int) (((int) (J3/100) +1)*3/4);
    }

  // line 1260                           add in month day, whole day, 
  //                                     and gregorian adjustment.
  J=J+ (int)(275*M/9)+D1+G*J3;

  // line 1270                           add in base offset, days according 
  //                                     to year number and gregorian days
  J=J+1721027+2*G+367*Y;

  // line 1280                           negative fractonal day?
  if (F<0) 
    {  
  // line 1290                          advance fraction and dec julian day
      F=F+1;
      J=J-1;
    }

  float T=(J-2451545)+F;

  // line 70 
  // line 80 
  float TT=T/36525+1;
  //printf("centuries since 1900: %f\n", TT);

  // line 90 

  // line 410 
  // line 420 
  float T0 = T/36525;
  
  // line 430 
  float S= 24110.5 + 8640184.813*T0;

  // line 440 
  S=S+86636.6*Z0+86400*L5;

  // line 450 
  S=S/86400;
  S=S-(int)S;

  // line 460 
  T0=S*360*DR;

  // line 470 
  // line 90 
  T=T+Z0;
  
  // line 100 
  // line 110 
  // line 120 
  sunpos(T, TT, Aa[0], Da[0]);

  // line 130 
  T=T+1; 
  
  // line 140 
  sunpos(T, TT, Aa[1], Da[1]);

  // line  150 
  if ( Aa[1] < Aa[0] )
    Aa[1]=Aa[1]+P2;
  
  // line  160 
  float Z1=DR*90.833;

  // line  170 
  S=sin(B5*DR);
  float C=cos(B5*DR);

  // line  180 
  float Z=cos(Z1);
  int M8=0;
  int W8=0;

  // line  190 
  float A0=Aa[0];
  float D0=Da[0];

  // line  200 
  float DA=Aa[1]-Aa[0];
  float DD=Da[1]-Da[0];
  
  float V0=0;
  float V1=0;
  float V2=0;
  float midmorningaz;

  // line  210 
  for (float C0=0; C0<=23; C0+=1)
    {
      //  
      float P=(C0+1)/24.0;
      
      //  
      float A2=Aa[0]+P*DA;
      float D2=Da[0]+P*DD;

      // line  240 
      // line  490 
      // line  500 
      float L0=T0+C0*K1;
      float L2=L0+K1;

      // line  510 
      float H0=L0-A0;
      float H2=L2-A2;

      // line  520 
      float H1=(H2+H0)/2;

      // line  530 
      float D1=(D2+D0)/2;

      // line  540 
      // line  550 
      if (C0<=0)
	{
	  // line  560 
	  V0=S*sin(D0)+C*cos(D0)*cos(H0)-Z;
	}

      // line  570 
      V2=S*sin(D2)+C*cos(D2)*cos(H2)-Z;

      // line  580 
      if (SGN(V0)!=SGN(V2))
	{
	  // line  590 
	  V1=S*sin(D1)+C*cos(D1)*cos(H1)-Z;
	  
	  // line  600 
	  float A=2*V2-4*V1+2*V0;
	  float B=4*V1-3*V0-V2;
	  
	  // line  610 
	  float D=B*B-4*A*V0;
	  if (D>=0)
	    {
	      float *timep;
	      float *azp;

	      // line  620 
	      D=sqrt(D);
	      
	      // line  630 
	      // line  640 
	      if (V0<0 && V2>0)
		{
		  //printf("Sunrise at: ");
		  timep = &rise;
		  azp = &riseaz;
		  M8=1;
		}
	      
	      // line  650 
	      // line  660 
	      if (V0>0 && V2<0) 
		{
		  //printf("Sunset at: ");
		  timep = &set;
		  azp = &setaz;
		  W8=1;
		}
	      
	      // line  670 
	      float E=(-B+D)/(2*A);
	      
	      // line  680 
	      if (E>1 || E<0) 
		E=(-B-D)/(2*A);

	      // line  690 
	      float T3=C0+E+1/120;

	      // line  700 
	      float H3= (int) T3;
	      float M3= (int) ((T3-H3)*60);

	      // line  710 
	      //printf("using %02d:%02d\n",(int)H3,(int)M3);
	      *timep = floor(H3)+floor(M3)/60;

	      // line  720 
	      float H7=H0+E*(H2-H0);

	      // line  730 
	      float N7=-cos(D1)*sin(H7);

	      // line  740 
	      float D7=C*sin(D1)-S*cos(D1)*cos(H7);

	      // line  750 
	      float AZ=atan(N7/D7)/DR;

	      // line  760 
	      if (D7<0) AZ=AZ+180;

	      // line  770 
	      if (AZ<0) AZ=AZ+360;

	      // line  780 
	      if(AZ>360) AZ=AZ-360;

	      // line  790 
	      //printf("using azimuth %03.1f\n",AZ);
	      *azp = AZ;
	    }	    
	}
      // line  800 
      // line 250 
      A0=A2;
      D0=D2;
      V0=V2;

      if (C0 == 10) midmorningaz = V2;
      
      // line 260 
      
    }
  // line  270 
  // line  820 
  // line  830 
  if (M8==0 || W8==0)
    {
      // line  870 
      if (midmorningaz<0) 
	{
	  //printf("Sun down all day\n");
	  setp = risep = 0;
          allday = -1;
	}

      // line  880 
      if (midmorningaz>0)
	{
	  //printf("Sun up all day\n");
	  setp = risep = 0;
          allday = 1;
	}
    }
  else
    {
      // line  840 
      if (M8==0) 
	{
	  //printf("No sunrise this date\n");
	  risep = 0;
	}

      // line  850 
      if (W8==0) 
	{
	  //printf("No sunset this date\n");
	  setp = 0;
	}

      // line  860 
    }
  // line 890 
  // line 280
}




// pulled in from today.cc so other apps could use it
float parseTimezone(int argc,char **argv)
{
  float zone;

  if (argc > 3 && (argv[3][0] == '-' || isdigit(argv[3][0])))
  {
      zone = atof (argv[3]);
  }
  else
  {
    if (argc > 3)
    {
        char buf[1024];
        sprintf(buf,"TZ=%s",argv[3]);
        putenv(buf);
    }
    tzset();
    zone = -((float)timezone)/3600.0;
/* add in DST  NO!!!!
    // tzset() is implied by localtime():
    //tzset();
    time_t gmt=time(NULL);
    struct tm *ptm=localtime(&gmt);
    zone = -((float)timezone)/3600.0;
    if (daylight && ptm->tm_isdst>0)
    {
      zone+=ptm->tm_isdst; // should we do this, or just add 1???
    }
// */
  }

  return zone;
}
