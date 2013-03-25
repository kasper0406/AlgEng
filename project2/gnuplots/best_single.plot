log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 7,4
set output "./best_single.pdf"

set xlabel "log2 input size"
set xtics 0, 1 rotate
set offset 1, 1

set ylabel "Running time [s]"

set ytics nomirror tc lt 1

set logscale y
set grid mytics

set key vert left top Left reverse
set pointsize 2

plot '../data/8192/row-tiled64x64 recursive-64(simd-tiled-bc)_column-tiled-64x64 recursive-64(generic-bc)_row-tiled64x64 recursive-64(simd-tiled-bc)_0.dat' using (log2($1)):($7) title "Recursive, tiled, 64x64, SIMD" axes x1y1 with linespoints, \
'../data/8192/row-tiled64x64 tiled-iterative(simd-tiled-bc)_column-tiled-64x64 tiled-iterative(simd-tiled-bc)_row-tiled64x64 tiled-iterative(simd-tiled-bc)_0.dat' using (log2($1)):($7) title "Iterative, tiled, 64x64, SIMD" axes x1y1 with linespoints, \
'../data/8192/z-curve-tiled strassen-32(32-simd-fixed-tiled-bc)_SIMD_z-curve-tiled strassen-32(32-simd-fixed-tiled-bc)_SIMD_z-curve-tiled strassen-32(32-simd-fixed-tiled-bc)_SIMD_0.dat' using (log2($1)):($7) title "Strassen, 32x32, SIMD" axes x1y1 with linespoints, \
'../data/2048/row-based naive_row-based naive_row-based naive_0.dat' using (log2($1)):($7) title "Naive, row/row" axes x1y1 with linespoints