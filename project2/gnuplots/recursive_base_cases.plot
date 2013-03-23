log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 7,4
set output "./recursive_base_cases.pdf"

set xlabel "log2 input size"
set xtics 0, 1 rotate
set offset 1, 1

set ylabel "Running time [s]"

set ytics nomirror tc lt 1

set grid mytics

set key vert left top reverse
set pointsize 2

plot '../data/bc/row-tiled8x8 recursive-8(tiled-bc)_column-tiled-8x8 recursive-8(generic-bc)_row-tiled8x8 recursive-8(tiled-bc)_0.dat' using (log2($1)):($7) title "8x8" axes x1y1 with linespoints, \
'../data/bc/row-tiled16x16 recursive-16(tiled-bc)_column-tiled-16x16 recursive-16(generic-bc)_row-tiled16x16 recursive-16(tiled-bc)_0.dat' using (log2($1)):($7) title "16x16" axes x1y1 with linespoints, \
'../data/bc/row-tiled32x32 recursive-32(tiled-bc)_column-tiled-32x32 recursive-32(generic-bc)_row-tiled32x32 recursive-32(tiled-bc)_0.dat' using (log2($1)):($7) title "32x32" axes x1y1 with linespoints, \
'../data/bc/row-tiled64x64 recursive-64(tiled-bc)_column-tiled-64x64 recursive-64(generic-bc)_row-tiled64x64 recursive-64(tiled-bc)_0.dat' using (log2($1)):($7) title "64x64" axes x1y1 with linespoints, \
'../data/bc/row-tiled128x128 recursive-128(tiled-bc)_column-tiled-128x128 recursive-128(generic-bc)_row-tiled128x128 recursive-128(tiled-bc)_0.dat' using (log2($1)):($7) title "128x128" axes x1y1 with linespoints