VERSION=0.51sdb

sinclude Makefile.local

DEF_LAT ?= 42.3778343
DEF_LON ?= -71.1063232
DEF_TZ ?= -5

LIB=ephem-${VERSION}
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
	  ephem.cc ephem.h year.cc today.cc x10events.cc \
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
	cp ephem.h ${DESTDIR}/usr/include/
	cp year today x10events ${DESTDIR}/usr/bin/

ephem.o: ephem.cc ephem.h
today.o: today.cc ephem.h
x10events.o: x10events.cc ephem.h



# plot target replaces year.sh (aka ephemyear.sh)
plot: plot.gif

plot.gif: plot.gnuplot rise-my.txt set-my.txt rise-off.txt set-off.txt
	@rm -f $@
	gnuplot   < $<   > $@

clean-plot:
	rm -f rise-off.txt set-off.txt rise-my.txt set-my.txt
	rm -f plot.gnuplot plot.gif
	rm -f getofftxt.sh  txt2mat.sh official.mat

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
plot \
  "rise-my.txt"   title "calculated rise", \
  "rise-off.txt"  title "official rise",   \
  "set-my.txt"    title "calculated set",  \
  "set-off.txt"   title "official set"
endef
export PLOTCOMMANDS
plot.gnuplot: Makefile
	@rm -f $@
	echo "$$PLOTCOMMANDS" >> $@

rise-off.txt: official.mat
	@rm -f $@
	awk '{print $$1}'   < $<   > $@

set-off.txt: official.mat
	@rm -f $@
	awk '{print $$2}'   < $<   > $@

rise-my.txt: year Makefile
	@rm -f $@
	./year ${DEF_LAT} ${DEF_LON} ${DEF_TZ} | awk '{print $$1}' > $@

set-my.txt: year Makefile
	@rm -f $@
	./year ${DEF_LAT} ${DEF_LON} ${DEF_TZ} | awk '{print $$2}' > $@


define TXT2MAT
#!/bin/sh
# process official data from:
#   http://aa.usno.navy.mil/data/docs/RS_OneYear.php
# for use by 'plot'
for month in `seq 0 11`; do
  cat "$$1" | awk '/^[0-9][0-9]/' | sed 's/            /  xxxx xxxx  /g' |
  awk 'function hm2h(hm) {return substr(hm,1,2)+(substr(hm,3,2)/60)}  BEGIN {mon='$$month'}  {r=$$(2+mon*2);s=$$(3+mon*2);if (r!=s && r!="xxxx") {printf "%05.2f %05.2f\\n",hm2h(r),hm2h(s)}}'
  echo "Month: $$month" >&2
done
endef
export TXT2MAT
txt2mat.sh: Makefile
	@rm -f $@
	echo "$$TXT2MAT" >> $@
	@chmod u+x $@

official.mat: official.txt txt2mat.sh  getofftxt.sh
	@rm -f $@
	./txt2mat.sh   $<   >> $@


# retrieve one day:
#   http://aa.usno.navy.mil/data/docs/RS_OneDay.php
#   curl -d 'FFX=1&ID=LinuxCurl&xxy=2010&xxm=11&xxd=15&st=ID&place=boise&ZZZ=END' http://aa.usno.navy.mil/cgi-bin/aa_pap.pl
# retrieve one year:
#   http://aa.usno.navy.mil/data/docs/RS_OneYear.php
#   curl -d 'FFX=1&xxy=2011&type=0&st=ID&place=boise&ZZZ=END' http://aa.usno.navy.mil/cgi-bin/aa_rstablew.pl
#   curl -d 'FFX=2&xxy=2011&type=0&place=Some+Place+at+W116+13%2C+N43+37&xx0=-1&xx1=116&xx2=13&yy0=1&yy1=43&yy2=37&zz1=7&zz0=-1&ZZZ=END' http://aa.usno.navy.mil/cgi-bin/aa_rstablew.pl

define GETOFFTXT
#!/bin/sh
# retrieve official data from:
#   http://aa.usno.navy.mil/data/docs/RS_OneYear.php
lat=$$1; shift
lon=$$1; shift
zon=$$1; shift
# year
test "$$1" && xxy=$$1 || xxy=$$(date +%Y)
# convert 1 to 4 digits to minutes, assumes the digits are the decimal portion
tomins() { d=$${1}0000; e=$${d#????}; d=$${d%$$e}; echo $$(((60* d +5000)/10000)); }
# lon (pos integer)
xx1=$${lon%.*}
# lon minutes (pos integer)
xx2=$$(tomins $${lon#*.})
# lon sign (-1=West/1)
xx0=1; test $$xx1 -lt 0 && xx0=-1 && xx1=$$((xx1 * -1))
# lat (pos integer)
yy1=$${lat%.*}
# lat minutes (pos integer)
yy2=$$(tomins $${lat#*.})
# lat sign (-1/1=North)
yy0=1; test $$yy1 -lt 0 && yy0=-1 && yy1=$$((yy1 * -1))
# time zone hours (pos integer)
zz1=$$zon
# time zone sign (-1=West/1)
zz0=1; test $$zz1 -lt 0 && zz0=-1 && zz1=$$((zz1 * -1))
curl -d 'FFX=2&xxy='$$xxy'&type=0&place=Some+Place&xx0='$$xx0'&xx1='$$xx1'&xx2='$$xx2'&yy0='$$yy0'&yy1='$$yy1'&yy2='$$yy2'&zz1='$$zz1'&zz0='$$zz0'&ZZZ=END' http://aa.usno.navy.mil/cgi-bin/aa_rstablew.pl
endef
export GETOFFTXT
getofftxt.sh: Makefile
	@rm -f $@
	echo "$$GETOFFTXT" >> $@
	@chmod u+x $@

# don't needlessly fetch, remove by hand to refetch
official.txt:
	./getofftxt.sh ${DEF_LAT} ${DEF_LON} ${DEF_TZ}   > $@
