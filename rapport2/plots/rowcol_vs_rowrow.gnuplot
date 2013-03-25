log2(x) = log(x)/log(2)
max(a,b) = (a > b) ? a : b

set terminal pdf enhanced font "Helvetica, 10" size 7,4
set output "./rowcol_vs_rowrow.pdf"

set xlabel "log2 input size"
set xtics 0, 1 rotate
set offset 1, 1

set ylabel "Improvement factor"

set ytics nomirror tc lt 1

set logscale y
set grid mytics

set key vert left top reverse
set pointsize 1

#'rowcol.dat' using (log2($1)):($7) title "Time" axes x1y1 with linespoints, \

plot '<paste rowrow_edited.dat rowcol.dat' using (log2($1)):($7/$23) title "Time improvement" axes x1y1 with linespoints, \
     '<paste rowrow_edited.dat rowcol.dat' using (log2($1)):($11/$27) title "L2 cache fault improvement" axes x1y1 with linespoints, \
     '<paste rowrow_edited.dat rowcol.dat' using (log2($1)):($13/$29) title "L3 cache fault improvement" axes x1y1 with linespoints, \
     '<paste rowrow_edited.dat rowcol.dat' using (log2($1)):($14/$30) title "Instruction improvement" axes x1y1 with linespoints
#'<paste rowrow.dat rowcol.dat' using (log2($1)):($11/$27) title "L2 cache misses" axes x1y2 with linespoints
     # 'rowcol.dat' using (log2($1)):($13) title "L3 cache misses" axes x1y2 with linespoints, \
