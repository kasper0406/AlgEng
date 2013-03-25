log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 7,4
set output "./amdahl.pdf"

set xlabel "# cores"
set xtics 0, 1 rotate
set offset 1, 1

set ylabel "Speedup"

set ytics nomirror tc lt 1

set key vert left top reverse
set pointsize 2

amdahl(n,p) = 1 / ((1 - p) + (p / n))

plot [1:20] [0:10] amdahl(x, 0.898) title "Amdahl's law, Strassen", \
amdahl(x, 0.977) title "Amdahl's law, Recursive + Iterative", \
'amdahl_strassen.dat' using ($1):($2) title "Strassen, 32x32" axes x1y1 with points, \
'amdahl_recursive.dat' using ($1):($2) title "Recursive, 64x64, tiled" axes x1y1 with points, \
'amdahl_iterative.dat' using ($1):($2) title "Iterative, 64x64, tiled" axes x1y1 with points