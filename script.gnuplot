set terminal X
set title  "Calculated sunrise and sunset"
set xlabel "Julian Day"
set ylabel "Hour of Day"
set xrange [0:365]
set yrange [0:24]
set data style lines
plot "/tmp/myrise.txt", "/tmp/myset.txt", "/tmp/rise.txt", "/tmp/set.txt"
! xmessage exit

