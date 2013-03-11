#!/opt/local/bin/gnuplot
log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 6,4
set output "../outputs/bfs_dfs_cachefaults.pdf"

set xlabel "Input size"
set ylabel "L2 Cache faults / query"
set xrange [-1:25]
#set yrange [0:720]

#set logscale y

set xtics 0,1,24

set key vert left top reverse
set pointsize 2

plot "../data/ipcm/ipcm_Btree_lin_1.dat" using (log2($2)):($9/10000000) title "BFS layout", \
     "../data/ipcm/ipcm_DFS_lin_1.dat" using (log2($2)):($9/10000000) title "DFS layout", \
     max(0, x - log2(2**16)) title "Expected BFS cache faults", \
     max(0, 0.53125 * (x - 16)) title "Expected DFS cache faults"
