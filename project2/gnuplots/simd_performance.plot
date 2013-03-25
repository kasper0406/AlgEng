log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 7,4
set output "./simd_performance.pdf"

set xlabel "log2 input size"
set xtics 0, 1 rotate
set offset 1, 1

set ylabel "Speedup w/SIMD"

set ytics nomirror tc lt 1

set grid mytics

set key vert left top Left reverse
set pointsize 2

plot [7:12] [1.5:3.5] "< paste '../data/4096/row-tiled64x64 recursive-64(tiled-bc)_column-tiled-64x64 recursive-64(generic-bc)_row-tiled64x64 recursive-64(tiled-bc)_0.dat' '../data/4096/row-tiled64x64 recursive-64(simd-tiled-bc)_column-tiled-64x64 recursive-64(generic-bc)_row-tiled64x64 recursive-64(simd-tiled-bc)_0.dat'" using (log2($1)):($7 / $23) title "Recursive, tiled, 64x64" axes x1y1 with linespoints, \
"< paste '../data/4096/z-curve-tiled strassen-32(32-fixed-tiled-bc)_z-curve-tiled strassen-32(32-fixed-tiled-bc)_z-curve-tiled strassen-32(32-fixed-tiled-bc)_0.dat' '../data/4096/z-curve-tiled strassen-32(32-simd-fixed-tiled-bc)_SIMD_z-curve-tiled strassen-32(32-simd-fixed-tiled-bc)_SIMD_z-curve-tiled strassen-32(32-simd-fixed-tiled-bc)_SIMD_0.dat'" using (log2($1)):($7 / $23) title "Strassen, 32x32" axes x1y1 with linespoints