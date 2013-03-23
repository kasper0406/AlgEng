log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 7,4
set output "./recursive_vs_strassen_performance.pdf"

set xlabel "log2 input size"
set xtics 0, 1 rotate
set offset 1, 1

set ylabel "Running time [s]"

set ytics nomirror tc lt 1

set logscale y
set grid mytics

set key vert left top reverse
set pointsize 2

plot '../data/bc/row-tiled64x64 recursive-64(tiled-bc)_column-tiled-64x64 recursive-64(generic-bc)_row-tiled64x64 recursive-64(tiled-bc)_0.dat' using (log2($1)):($7) title "Recursive, 64x64" axes x1y1 with linespoints, \
'../data/bc/z-curve-tiled strassen-32(32-fixed-tiled-bc)_z-curve-tiled strassen-32(32-fixed-tiled-bc)_z-curve-tiled strassen-32(32-fixed-tiled-bc)_0.dat' using (log2($1)):($7) title "Strassen, 32x32" axes x1y1 with linespoints