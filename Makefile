VERSION=0.41sdb

DEF_LAT=42.3778343
DEF_LON=-71.1063232

CXXFLAGS=-D VERSION=$(VERSION) -D DLON=$(DEF_LON) -D DLAT=$(DEF_LAT)

all: year today x10events

clean:
	rm -f libephem.a ephem.o
	rm -f year.o today.o x10events.o

realclean: clean
	rm -f year today x10events

libephem.a: ephem.o
	ar r libephem.a ephem.o

year: libephem.a year.o
	$(CXX) year.o -o year -L. -lephem -lm 

today: libephem.a today.o
	$(CXX) ephem.o today.o -o today -L. -lephem -lm 

x10events: libephem.a x10events.o
	$(CXX) ephem.o x10events.o -o x10events -L. -lephem -lm 

TARTARGETS=BUGS COPYING README FAQ ChangeLog sample.cron \
	  riseset.mat riseset.txt \
	  astrotwilight.txt \
	  civiltwilight.mat civiltwilight.txt \
	  sunup.bas year.sh Makefile \
	  ephem.cc ephem.h year.cc today.cc x10events.cc x10events.sh

TARFILES= $(addprefix ephem-$(VERSION)/, $(TARTARGETS))

archive: realclean $(TARTARGETS)
	cd ..; tar czvf ephem-$(VERSION).tar.gz $(TARFILES)

ephem.o: ephem.cc ephem.h
today.o: today.cc ephem.h
x10events.o: x10events.cc ephem.h


