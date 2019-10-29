all: year today x10events

VERSION=0.50

LIB=x10ephem-${VERSION}
ARCHIVE=libx10ephem-${VERSION}.a

${ARCHIVE}: ephem.o
	ar r ${ARCHIVE} ephem.o

year: ${ARCHIVE} year.o
	$(CXX) year.o -o year -L. -lx10ephem-${VERSION} -lm 

today: ${ARCHIVE} today.o
	$(CXX) ephem.o today.o -o today -L. -lx10ephem-${VERSION} -lm 

x10events: ${ARCHIVE} x10events.o
	$(CXX) ephem.o x10events.o -o x10events -L. -lx10ephem-${VERSION} -lm 

TARTAGETS=BUGS COPYING README FAQ CHANGELOG Makefile        \
	  riseset.mat riseset.txt year.sh x10events.sh      \
          ephem.cc x10ephem.h year.cc today.cc x10events.cc \
          ephem.spec sample.cron

TARFILES= $(addprefix x10ephem-${VERSION}/, $(TARTAGETS))

ephemtar: $(TARTAGETS)
	cd ..; tar czvf x10ephem-${VERSION}/x10ephem-${VERSION}.tar.gz $(TARFILES)

install: ${ARCHIVE} year today x10events sample.cron
	- mkdir -p ${DESTDIR}/usr/lib/
	- mkdir -p ${DESTDIR}/usr/bin/
	- mkdir -p ${DESTDIR}/usr/include/
	cp ${ARCHIVE} ${DESTDIR}/usr/lib/
	cp x10ephem.h ${DESTDIR}/usr/include/
	cp year today x10events ${DESTDIR}/usr/bin/

ephem.o: ephem.cc x10ephem.h
today.o: today.cc x10ephem.h
x10events.o: x10events.cc x10ephem.h


