log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 7,4
set output "./recursive_cache.pdf"

set xlabel "log2 input size"
set xtics 0, 1 rotate
set offset 1, 1

set ylabel "Cache misses"

set ytics nomirror tc lt 1

set logscale y
set grid mytics

set key vert left top reverse
set pointsize 2

plot '../data/4096/row-tiled64x64 recursive-64(tiled-bc)_column-tiled-64x64 recursive-64(generic-bc)_row-tiled64x64 recursive-64(tiled-bc)_0.dat' using (log2($1)):($11) title "Tiled, L2" axes x1y1 with linespoints, \
'../data/4096/row-tiled64x64 recursive-64(tiled-bc)_column-tiled-64x64 recursive-64(generic-bc)_row-tiled64x64 recursive-64(tiled-bc)_0.dat' using (log2($1)):($13) title "Tiled, L3" axes x1y1 with linespoints, \
'../data/4096/z-curve recursive-8(8-zlayout-bc)_z-curve recursive-8(8-zlayout-bc)_z-curve recursive-8(8-zlayout-bc)_0.dat' using (log2($1)):($11) title "Z-curve, L2" axes x1y1 with linespoints, \
'../data/4096/z-curve recursive-8(8-zlayout-bc)_z-curve recursive-8(8-zlayout-bc)_z-curve recursive-8(8-zlayout-bc)_0.dat' using (log2($1)):($13) title "Z-curve, L3" axes x1y1 with linespoints