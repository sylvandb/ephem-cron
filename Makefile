VERSION=0.50sdb

DEF_LAT=42.3778343
DEF_LON=-71.1063232

LIB=x10ephem-${VERSION}
ARCHIVE=lib${LIB}.a

CXXFLAGS=-D VERSION=${VERSION} -D DLAT=$(DEF_LAT) -D DLON=$(DEF_LON)



all: year today x10events


clean:
	rm -f ${ARCHIVE} ephem.o
	rm -f year.o today.o x10events.o

realclean: clean
	rm -f year today x10events suntime


${ARCHIVE}: ephem.o
	ar r ${ARCHIVE} ephem.o


year: ${ARCHIVE} year.o
	$(CXX) year.o -o year -L. -l${LIB} -lm

today: ${ARCHIVE} today.o
	$(CXX) ephem.o today.o -o today -L. -l${LIB} -lm

x10events: ${ARCHIVE} x10events.o
	$(CXX) ephem.o x10events.o -o x10events -L. -l${LIB} -lm


suntime: suntime.c
	$(CXX) $(CXXFLAGS) -o suntime suntime.c -L. -lm

TARTARGETS=BUGS COPYING README FAQ CHANGELOG Makefile     \
	  riseset.mat riseset.txt year.sh x10events.sh   \
	  ephem.cc x10ephem.h year.cc today.cc x10events.cc \
	  sunup.bas suntime.c year.sh Makefile \
	  ephem.spec sample.cron
	  #astrotwilight.txt civiltwilight.mat civiltwilight.txt \

TARFILES= $(addprefix ${LIB}/, $(TARTARGETS))

ephemtar: $(TARTARGETS)
	cd ..; tar czvf ${LIB}/${LIB}.tar.gz $(TARFILES)

archive: realclean ephemtar


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


