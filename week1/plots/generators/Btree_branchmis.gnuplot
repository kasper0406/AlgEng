#!/opt/local/bin/gnuplot
log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 6,4
set output "../outputs/Btree_branchmis.pdf"

set xlabel "Input size"
set ylabel "Branch mispredictions / query"
set xrange [-1:25]
#set yrange [0.04:0.22]

set xtics 0,1,24

set key vert right top reverse
set pointsize 2

plot "../data/counters1/counters1_Btree_bs_16.dat" using (log2($2)):($11/10000000) title "BFS bs (d = 17)", \
     "../data/counters1/counters1_Btree_lin_16.dat" using (log2($2)):($11/10000000) title "BFS lin (d = 17)", \
     "../data/counters1/counters1_Btree_bs_32.dat" using (log2($2)):($11/10000000) title "BFS bs (d = 33)", \
     "../data/counters1/counters1_Btree_lin_32.dat" using (log2($2)):($11/10000000) title "BFS lin (d = 33)"
