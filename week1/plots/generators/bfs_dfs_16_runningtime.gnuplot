#!/opt/local/bin/gnuplot
log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 6,4
set output "../outputs/bfs_dfs_16_runningtime.pdf"

set xlabel "Input size"
set ylabel "Normalized running time [s] / 10M queries"
set xrange [-1:25]
#set yrange [0.04:0.19]

set xtics 0,1,24

set key vert left top reverse
set pointsize 2

plot "../data/ipcm/ipcm_bs_1.dat" using (log2($2)):($6/max(1, log2($2))) title "Binary Search", \
     "../data/ipcm/ipcm_Btree_bs_32.dat" using (log2($2)):($6/max(1, log2($2))) title "BFS bs (d = 33)", \
     "../data/ipcm/ipcm_Btree_lin_16.dat" using (log2($2)):($6/max(1, log2($2))) title "BFS lin (d = 17)", \
     "../data/ipcm/ipcm_DFS_lin_16.dat" using (log2($2)):($6/max(1, log2($2))) title "DFS lin (d = 17)"