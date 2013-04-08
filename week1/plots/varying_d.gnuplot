#!/opt/local/bin/gnuplot
log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 6,4
set output "./varying_d.pdf"

set xlabel "d-1"
set ylabel "Median time [s]"
set xrange [-1:35]
#set yrange [0:720]

#set logscale y

set xtics 0,2,35

set key vert left top reverse
set pointsize 2

plot "./varying_d.txt" using ($1):($4) title "Measurements"