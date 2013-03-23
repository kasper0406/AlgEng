log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 7,4
set output "./strassen_base_cases.pdf"

set xlabel "log2 input size"
set xtics 0, 1 rotate
set offset 1, 1

set ylabel "Running time [s]"

set ytics nomirror tc lt 1

set grid mytics

set key vert left top reverse
set pointsize 2

plot '../data/bc/z-curve-tiled strassen-8(8-fixed-tiled-bc)_z-curve-tiled strassen-8(8-fixed-tiled-bc)_z-curve-tiled strassen-8(8-fixed-tiled-bc)_0.dat' using (log2($1)):($7) title "8x8" axes x1y1 with linespoints, \
'../data/bc/z-curve-tiled strassen-16(16-fixed-tiled-bc)_z-curve-tiled strassen-16(16-fixed-tiled-bc)_z-curve-tiled strassen-16(16-fixed-tiled-bc)_0.dat' using (log2($1)):($7) title "16x16" axes x1y1 with linespoints, \
'../data/bc/z-curve-tiled strassen-32(32-fixed-tiled-bc)_z-curve-tiled strassen-32(32-fixed-tiled-bc)_z-curve-tiled strassen-32(32-fixed-tiled-bc)_0.dat' using (log2($1)):($7) title "32x32" axes x1y1 with linespoints, \
'../data/bc/z-curve-tiled strassen-64(64-fixed-tiled-bc)_z-curve-tiled strassen-64(64-fixed-tiled-bc)_z-curve-tiled strassen-64(64-fixed-tiled-bc)_0.dat' using (log2($1)):($7) title "64x64" axes x1y1 with linespoints, \
'../data/bc/z-curve-tiled strassen-128(128-fixed-tiled-bc)_z-curve-tiled strassen-128(128-fixed-tiled-bc)_z-curve-tiled strassen-128(128-fixed-tiled-bc)_0.dat' using (log2($1)):($7) title "128x128" axes x1y1 with linespoints