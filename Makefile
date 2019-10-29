VERSION=0.51sdb

sinclude Makefile.local

DEF_LAT ?= 42.3778343
DEF_LON ?= -71.1063232
DEF_TZ ?= -5

LIB=x10ephem-${VERSION}
ARCHIVE=lib${LIB}.a

CXXFLAGS=-D VERSION=${VERSION} -D DLAT=${DEF_LAT} -D DLON=${DEF_LON} -D DTZ=${DEF_TZ}



all: year today x10events


clean: clean-plot
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



# plot target replaces year.sh (aka ephemyear.sh)
plot: plot.gif

plot.gif: plot.gnuplot rise-my.txt set-my.txt rise-off.txt set-off.txt
	gnuplot   < $<   > $@

clean-plot:
	rm -f rise-off.txt set-off.txt rise-my.txt set-my.txt plot.gnuplot plot.gif

define PLOTCOMMANDS
set terminal gif
set title  "Calculated sunrise and sunset"
set xlabel "Julian Day"
set ylabel "Hour of Day"
set xrange [0:365]
set yrange [0:24]
set style data lines
plot \\
  "rise-my.txt"   title "calculated rise", \\
  "rise-off.txt"  title "official rise",   \\
  "set-my.txt"    title "calculated set",  \\
  "set-off.txt"   title "official set"
endef
export PLOTCOMMANDS
plot.gnuplot: Makefile
	rm -f $@
	echo "$$PLOTCOMMANDS" >> $@

rise-off.txt: riseset.mat
	awk '{print $$1}'   < $<   > $@

set-off.txt: riseset.mat
	awk '{print $$2}'   < $<   > $@

rise-my.txt: year
	./year 42.3778343 -71.1063232 -5 | awk '{print $$1}' > $@

set-my.txt: year
	./year 42.3778343 -71.1063232 -5 | awk '{print $$2}' > $@
