#ifndef lint
static char rcsid[] = "$Id$";
#endif

/*
 * Suntime - calculate sunset or sunrise, or the requested twilight hour
 * http://www.tworoads.net/~srp/ha/ + ftp://ftp.tworoads.net/pub/ha/suntime.c
 * Taken largly from sun_time by Bruce Winters.
 */

/*
 * $Log$
 * Revision 1.1  2008/12/21 01:12:01  sylvan
 * more tweak comments, added suntime.c just to keep track of it
 *
 * Revision 1.1  1999/08/18 04:58:18  srp
 * Initial revision
 *
 * 
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

static time_t t;

int adjust_dst(int);
double normalize(double);

int rise = -1;
int verbose = 0;

double D;

#define OPTSTRING	"ancrsvlt:g:z:"

main(int argc, char **argv)
{

    int hour, min, sec;
    int yday;
    int option;
    char tmp[80];
    double A, B, C, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V;
    char * progname = argv[0];
    double  latitude, longitude, time_zone;
    struct tm * ltm;

    /* 2902 N. Puget Sound Ave., Tacoma, WA 98407 */
    latitude = 47.27;
    longitude = 122.48;
    time_zone = 8;
    tzset();
    time_zone = ((float)timezone)/3600.0;
#ifdef DLON
    longitude = - DLON;
#endif
#ifdef DLAT
    latitude = DLAT;
#endif

    /* To get time of     sunrise or sunset, use R = -.0145439 */
    R = -.0145439;

    /* Process arguments */
    while ((option = getopt(argc, argv, OPTSTRING)) != EOF) {
	    switch(option) {
	    /* To get time of astronomical twilight, use R = -.309017 */
	    case 'a':
		    R = -0.309017;
		    break;
	    /* To get time of     nautical twilight, use R = -.207912 */
	    case 'n':
		    R = -0.207912;
		    break;
	    /* To get time of        civil twilight, use R = -.104528 */
	    case 'c':
		    R = -0.104528;
		    break;
	    case 'r':
		    rise = 1;
		    break;
	    case 's':
		    rise = 0;
		    break;
	    case 'v':
		    verbose++;
		    break;
	    case 't':
		    latitude = atof(optarg);
		    break;
	    case 'g':
		    longitude = atof(optarg);
		    break;
	    case 'z':
		    time_zone = atoi(optarg);
		    break;
	    }
    }

    if (rise == -1) {
	    fprintf(stderr,"%s: must use either -r (sunrise) or -s (sunset)\n"
			    " with -a (astronomical twilight)\n"
			    "  or -n (nautical twilight)\n"
			    "  or -c (civil twilight)\n"
			    " with -v (verbose)\n"
			    " with -t (lat) -g (long) -z (zone)\n"
		    ,progname);
	    exit(1);
    }
    if (verbose) {
	    printf("latitude & longitude are now: %-5.2lf, %-5.2lf, timezone %-5.2lf\n",
		   latitude, longitude, time_zone);
    }

    A = 1.5708;
    B = M_PI;
    C = 4.71239;
    D = 2 * M_PI;
    E = 0.0174533 * latitude;
    F = 0.0174533 * longitude;
    G = 0.261799  * time_zone;

    t = time(NULL);
    ltm = localtime(&t);
    yday = ltm->tm_yday;

    J = (rise) ? A : C;
    K = yday + ((J + F) / D);
    L = (K * .017202) - .0574039;   /* Solar Mean Anomoly */
    M = L + .0334405 * sin(L);      /* Solar True Longitude */
    M += 4.93289 + (3.49066E-04) * sin(2 * L);
    M = normalize(M);              /* Quadrant Determination */
    if (((M / A) - (int) floor(M / A)) == 0) {
	M += 4.84814E-06;
    }
    P = sin(M) / cos(M);           /* Solar Right Ascension */
    P = atan2(.91746 * P, 1);
    if (M > C) {                   /* Quadrant Adjustment */
        P += D;
    } else if (M > A) {
        P += B;
    }
    Q = .39782 * sin(M);          /* Solar Declination */
    Q = Q / sqrt(-Q * Q + 1);     /* This is how the original author wrote it! */
    Q = atan2(Q, 1);

    S = R - (sin(Q) * sin(E));
    S = S / (cos(Q) * cos(E));
    if (S > 1 || S < -1) {
	exit(0);
    }

    S = S / sqrt(-S * S + 1);
    S = A - atan2(S, 1);
    if (rise) {
	S = D - S;
    }
    T = S + P - 0.0172028 * K - 1.73364;  /* Local apparent time */
    U = T + F;                     /* Universal timer */
    V = U - G;                     /* Wall clock time */
    V = normalize(V);
    V = V * 3.81972;
    hour = (int) floor(V);
    min  = (int) floor((V - hour) * 60);
    sec  = (int) floor((V - hour - min/60) * 3600);
    hour = adjust_dst(hour);

    (void) sprintf(tmp, "%d:%02d", hour ,min);
    puts(tmp);
    exit(0);

}


double normalize(double Z)
{

    while (Z < 0) {
	Z = Z + D;
    }

    while (Z >= D) {
	Z = Z - D;
    }

    return Z;
}

int adjust_dst (int hour)
{
    
    /* Note: jan -> month=0   sun -> wday=0 */
    /* First Sunday in April, Last in October */
    struct tm * ltm = localtime(&t);

    if ((ltm->tm_mon > 3  && ltm->tm_mon < 9) ||
	(ltm->tm_mon == 3 && (ltm->tm_mday - ltm->tm_wday > 0)) ||
	(ltm->tm_mon == 9 && (ltm->tm_mday - ltm->tm_wday < 25))) {
	hour++;
    }
    return hour;
}

