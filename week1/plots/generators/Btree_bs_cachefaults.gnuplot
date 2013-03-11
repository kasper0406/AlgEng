#!/opt/local/bin/gnuplot
log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 6,4
set output "../outputs/Btree_bs_cachefaults.pdf"

set xlabel "Input size"
set ylabel "L2 Cache faults / query"
set xrange [-1:25]
#set yrange [0:720]

#set logscale y

set xtics 0,1,24

set key vert left top reverse
set pointsize 2

plot "../data/ipcm/ipcm_Btree_bs_8.dat" using (log2($2)):($9/10000000) title "BFS (d = 9)", \
     "../data/ipcm/ipcm_Btree_bs_16.dat" using (log2($2)):($9/10000000) title "BFS (d = 17)", \
     "../data/ipcm/ipcm_Btree_bs_32.dat" using (log2($2)):($9/10000000) title "BFS (d = 33)", \
     "../data/ipcm/ipcm_Btree_bs_64.dat" using (log2($2)):($9/10000000) title "BFS (d = 65)"
