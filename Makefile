all: year today x10events

libephem.a: ephem.o
	ar r libephem.a ephem.o

year: libephem.a year.o
	$(CXX) year.o -o year -L. -lephem -lm 

today: libephem.a today.o
	$(CXX) ephem.o today.o -o today -L. -lephem -lm 

x10events: libephem.a x10events.o
	$(CXX) ephem.o x10events.o -o x10events -L. -lephem -lm 

TARTAGETS=BUGS COPYING README FAQ CHANGELOG Makefile     \
	  riseset.mat riseset.txt year.sh x10events.sh   \
          ephem.cc ephem.h year.cc today.cc x10events.cc

TARFILES= $(addprefix ephem-0.20/, $(TARTAGETS))

ephem.tar.gz: $(TARTAGETS)
	cd ..; tar czvf ephem-0.20/ephem.tar.gz $(TARFILES)

ephem.o: ephem.cc ephem.h
today.o: today.cc ephem.h
x10events.o: x10events.cc ephem.h


