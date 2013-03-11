#!/opt/local/bin/gnuplot
log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 6,4
set output "../outputs/bs_cachefaults.pdf"

set xlabel "Input size"
set ylabel "Cache faults / query"
set xrange [-1:25]
#set yrange [0:720]

#set logscale y

set xtics 0,1,24

set key vert left top reverse
set pointsize 2

plot "../data/ipcm/ipcm_bs_1.dat" using (log2($2)):($9/10000000) title "L2 Cache Faults", \
     "../data/ipcm/ipcm_bs_1.dat" using (log2($2)):($11/10000000) title "L3 Cache Faults", \
     max(0, x - log2(16) - log2(2**16 / 16)) title "Expected L2 cache faults", \
     max(0, x - log2(16) - log2(2**20 / 16)) title "Expected L3 cache faults"
     
