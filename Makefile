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
	@rm -f $@
	gnuplot   < $<   > $@

clean-plot:
	rm -f rise-off.txt set-off.txt rise-my.txt set-my.txt plot.gnuplot plot.gif

define PLOTCOMMANDS
set terminal gif
set title  "Calculated Sunrise and Sunset\\nAdd any local adjustment for Daylight Time"
set xlabel "Day of Year"
set ylabel "Hour of Day"
set xrange [0:366]
set xtics ( \
	"1Jan" 0, "1Jul" 182, "31Dec" 365, \
	""  31,  ""  60,  ""  90,  "" 121,  "" 151, \
	"" 213,  "" 243,  "" 274,  "" 304,  "" 335)
set yrange [0:24]
set ytics ( \
	"midnight" 0, "3am"  3, "6am"  6, "9am"  9, \
	"noon"    12, "3pm" 15, "6pm" 18, "9pm" 21, \
	""  1,  ""  2,   ""  4,  ""  5,   ""  7,  ""  8, "" 10, "" 11, \
	"" 13,  "" 14,   "" 16,  "" 17,   "" 19,  "" 20, "" 22, "" 23)
set grid
set style data lines
plot \\
  "rise-my.txt"   title "calculated rise", \\
  "rise-off.txt"  title "official rise",   \\
  "set-my.txt"    title "calculated set",  \\
  "set-off.txt"   title "official set"
endef
export PLOTCOMMANDS
plot.gnuplot: Makefile
	@rm -f $@
	echo "$$PLOTCOMMANDS" >> $@

rise-off.txt: riseset.mat
	@rm -f $@
	awk '{print $$1}'   < $<   > $@

set-off.txt: riseset.mat
	@rm -f $@
	awk '{print $$2}'   < $<   > $@

rise-my.txt: year Makefile
	@rm -f $@
	./year ${DEF_LAT} ${DEF_LON} ${DEF_TZ} | awk '{print $$1}' > $@

set-my.txt: year Makefile
	@rm -f $@
	./year ${DEF_LAT} ${DEF_LON} ${DEF_TZ} | awk '{print $$2}' > $@

# http://aa.usno.navy.mil/data/docs/RS_OneYear.php
# http://aa.usno.navy.mil/cgi-bin/aa_rstablew.pl
# POSTDATA=FFX=1&xxy=2011&type=0&st=ID&place=boise&ZZZ=END
# POSTDATA=FFX=2&xxy=2011&type=0&place=a+post+--+W116+13%2C+N43+37&xx0=-1&xx1=116&xx2=13&yy0=1&yy1=43&yy2=37&zz1=7&zz0=-1&ZZZ=END
#local-off.txt:
#	curl blah, blah, blah...
