log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 7,4
set output "./parallel_speedup.pdf"

set xlabel "# cores"
set xtics 0, 1 rotate
set offset 1, 1

set ylabel "Speedup"

set ytics nomirror tc lt 1

set key vert left top reverse
set pointsize 2

amdahl(n,p) = 1 / ((1 - p) + (p / n))

plot 'parallel_speedup_strassen_simd.dat' using 2:xticlabels(1) title "Strassen, 32x32, SIMD" axes x1y1 with points, \
'parallel_speedup_recursive_simd.dat' using 2:xticlabels(1) title "Recursive, 64x64, tiled, SIMD" axes x1y1 with points, \
'parallel_speedup_iterative_simd.dat' using 2:xticlabels(1) title "Iterative, 64x64, tiled, SIMD" axes x1y1 with points, \
'parallel_speedup_strassen.dat' using 2:xticlabels(1) title "Strassen, 32x32" axes x1y1 with points, \
'parallel_speedup_recursive.dat' using 2:xticlabels(1) title "Recursive, 64x64, tiled" axes x1y1 with points, \
'parallel_speedup_iterative.dat' using 2:xticlabels(1) title "Iterative, 64x64, tiled" axes x1y1 with points