log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 7,4
set output "./parallel_speedup.pdf"

set xlabel "# cores"

set ylabel "Speedup"

set ytics nomirror tc lt 1

set key vert left top reverse
set pointsize 2

amdahl(n,p) = 1 / ((1 - p) + (p / n))

plot 'parallel_speedup_strassen_simd.dat' using 2:xticlabels(1) title "Strassen, 32x32, SIMD" with linepoints, \
'parallel_speedup_recursive_simd.dat' using 2:xticlabels(1) title "Recursive, 64x64, tiled, SIMD" with linepoints, \
'parallel_speedup_iterative_simd.dat' using 2:xticlabels(1) title "Iterative, 64x64, tiled, SIMD" with linepoints, \
'parallel_speedup_strassen.dat' using 2:xticlabels(1) title "Strassen, 32x32g" with linepoints, \
'parallel_speedup_recursive.dat' using 2:xticlabels(1) title "Recursive, 64x64, tiled" with linepoints, \
'parallel_speedup_iterative.dat' using 2:xticlabels(1) title "Iterative, 64x64, tiled" with linepoints