log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 7,4
set output "./iterative_cache.pdf"

set xlabel "log2 input size"
set xtics 0, 1 rotate
set offset 1, 1

set ylabel "Cache faults"

set ytics nomirror tc lt 1

set logscale y
set grid mytics

set key vert left top Left reverse
set pointsize 2

plot '../data/4096/row-tiled64x64 recursive-64(tiled-bc)_column-tiled-64x64 recursive-64(generic-bc)_row-tiled64x64 recursive-64(tiled-bc)_0.dat' using (log2($1)):($11) title "Recursive, tiled, 64x64, L2" axes x1y1 with linespoints, \
'../data/4096/row-tiled64x64 recursive-64(tiled-bc)_column-tiled-64x64 recursive-64(generic-bc)_row-tiled64x64 recursive-64(tiled-bc)_0.dat' using (log2($1)):($13) title "Recursive, tiled, 64x64, L3" axes x1y1 with linespoints, \
'../data/4096/row-tiled64x64 tiled-iterative(tiled-bc)_column-tiled-64x64 tiled-iterative(tiled-bc)_row-tiled64x64 tiled-iterative(tiled-bc)_0.dat' using (log2($1)):($11) title "Iterative, tiled, 64x64, L2" axes x1y1 with linespoints, \
'../data/4096/row-tiled64x64 tiled-iterative(tiled-bc)_column-tiled-64x64 tiled-iterative(tiled-bc)_row-tiled64x64 tiled-iterative(tiled-bc)_0.dat' using (log2($1)):($13) title "Iterative, tiled, 64x64, L3" axes x1y1 with linespoints