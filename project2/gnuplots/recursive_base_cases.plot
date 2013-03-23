log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 7,4
set output "./recursive_base_cases.pdf"

set xlabel "log2 input size"
set xtics 0, 1 rotate
set offset 1, 1

set ylabel "Normalized running time [s]"

set ytics nomirror tc lt 1

set logscale y
set grid mytics

set key vert left top reverse
set pointsize 2

plot '../data/bc/row-tiled8x8 recursive-8(tiled-bc)_column-tiled-8x8 recursive-8(generic-bc)_row-tiled8x8 recursive-8(tiled-bc)_0.dat' using (log2($1)):($7) title "Time" axes x1y1 with linespoints