log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 7,4
set output "./recursive_vs_strassen_cache.pdf"

set xlabel "log2 input size"
set xtics 0, 1 rotate
set offset 1, 1

set ylabel "Cache misses"

set ytics nomirror tc lt 1

set logscale y
set grid mytics

set key vert left top Left reverse
set pointsize 2

plot '../data/bc/row-tiled16x16 recursive-16(tiled-bc)_column-tiled-16x16 recursive-16(generic-bc)_row-tiled16x16 recursive-16(tiled-bc)_0.dat' using (log2($1)):($11) title "Recursive, tiled, 16x16, L2" axes x1y1 with linespoints, \
'../data/bc/row-tiled16x16 recursive-16(tiled-bc)_column-tiled-16x16 recursive-16(generic-bc)_row-tiled16x16 recursive-16(tiled-bc)_0.dat' using (log2($1)):($13) title "Recursive, tiled, 16x16, L3" axes x1y1 with linespoints, \
'../data/bc/z-curve-tiled strassen-16(16-fixed-tiled-bc)_z-curve-tiled strassen-16(16-fixed-tiled-bc)_z-curve-tiled strassen-16(16-fixed-tiled-bc)_0.dat' using (log2($1)):($11) title "Strassen, 16x16, L2" axes x1y1 with linespoints, \
'../data/bc/z-curve-tiled strassen-16(16-fixed-tiled-bc)_z-curve-tiled strassen-16(16-fixed-tiled-bc)_z-curve-tiled strassen-16(16-fixed-tiled-bc)_0.dat' using (log2($1)):($13) title "Strassen, 16x16, L3" axes x1y1 with linespoints